#!/usr/bin/env python3
import json, re, os, sys, statistics, glob, datetime, shutil

def parse_time_file(path):
    # Parse /usr/bin/time -v output for Elapsed (wall clock) and Maximum resident set size
    wall = None; rss = None
    with open(path, 'r', errors='ignore') as f:
        for line in f:
            if 'Elapsed (wall clock) time' in line:
                # Format like: 0:12.34 or 1:02:03
                val = line.split(':',1)[1].strip()
                parts = val.split(':')
                try:
                    if len(parts) == 3:
                        wall = int(parts[0])*3600 + int(parts[1])*60 + float(parts[2])
                    elif len(parts) == 2:
                        wall = int(parts[0])*60 + float(parts[1])
                    else:
                        wall = float(parts[0])
                except: pass
            if 'Maximum resident set size' in line:
                try: rss = float(line.split(':',1)[1].strip())
                except: pass
    return wall, rss

def gather_build_metrics(root):
    out = {}
    for phase in ['clean','incr','incr_after_touch']:
        timefile = os.path.join(root, f'build_{phase}.time' if phase!='incr_after_touch' else 'build_incr_after_touch.time')
        if os.path.exists(timefile):
            wall,rss = parse_time_file(timefile)
            out[phase] = {'wall_s': wall, 'max_rss_kb': rss}
    return out

def gather_sizes(build_dir):
    sizes = {}
    for path in glob.glob(os.path.join(build_dir, '**'), recursive=True):
        if os.path.isfile(path) and (path.endswith(('.a','.so','.dylib','.dll')) or os.access(path, os.X_OK)):
            try: sizes[path] = os.path.getsize(path)
            except: pass
    return sizes

def summarize_bench_jsons(bench_dir):
    # Google Benchmark JSON aggregator: compute median of real_time per name
    results = {}
    for p in glob.glob(os.path.join(bench_dir, '*.json')):
        try:
            data = json.load(open(p))
            for b in data.get('benchmarks', []):
                name = b.get('name'); t = b.get('real_time')
                if name and isinstance(t,(int,float)):
                    results.setdefault(name, []).append(t)
        except Exception:
            pass
    summary = {k: statistics.median(v) for k,v in results.items()}
    return summary

def load_env():
    return {
        'os': os.uname().sysname if hasattr(os, 'uname') else 'unknown',
        'python': sys.version.split()[0],
        'datetime': datetime.datetime.utcnow().isoformat()+'Z'
    }

def main():
    report_date = datetime.datetime.utcnow().strftime('%Y%m%d')
    raw_root = os.path.join('docs','perf','raw',report_date)
    out_md = os.path.join('docs','perf', f'opt-report-{report_date}.md')
    os.makedirs(os.path.dirname(out_md), exist_ok=True)

    env = load_env()
    summaries = {}
    for preset in ['baseline','optimized']:
        logs_dir = os.path.join(raw_root, preset)
        summaries[preset] = {
            'build': gather_build_metrics(logs_dir),
            'bench': summarize_bench_jsons(os.path.join(logs_dir, 'bench')),
        }

    # Compute speedups (lower is better for time)
    speedups = {}
    base_bench = summaries.get('baseline', {}).get('bench', {})
    opt_bench = summaries.get('optimized', {}).get('bench', {})
    common = sorted(set(base_bench) & set(opt_bench))
    for k in common:
        if base_bench[k] > 0:
            speedups[k] = base_bench[k] / opt_bench[k]

    # Write Markdown
    lines = []
    lines.append(f"# Optimization Evaluation Report ({report_date})")
    lines.append("")
    lines.append("## Environment")
    lines.append(f"- OS: {env['os']}")
    lines.append(f"- Python: {env['python']}")
    lines.append("")
    lines.append("## Build Results")
    for preset in ['baseline','optimized']:
        lines.append(f"### {preset.capitalize()}")
        build = summaries[preset].get('build',{})
        for phase,vals in build.items():
            lines.append(f"- {phase}: wall={vals.get('wall_s')}s, max_rss={vals.get('max_rss_kb')} KB")
        lines.append("")
    if common:
        geo = statistics.geometric_mean([speedups[k] for k in common if speedups[k]>0])
        lines.append("## Runtime Benchmarks")
        lines.append(f"- Geometric mean speedup (optimized vs baseline): {geo:.3f}x")
        lines.append("")
        lines.append("| Benchmark | Baseline (ns) | Optimized (ns) | Speedup |")
        lines.append("|---|---:|---:|---:|")
        for k in common:
            lines.append(f"| {k} | {base_bench[k]:.0f} | {opt_bench[k]:.0f} | {speedups[k]:.3f}x |")
    else:
        lines.append("## Runtime Benchmarks")
        lines.append("_No comparable benchmark data found._")
    with open(out_md,'w') as f:
        f.write("\\n".join(lines))
    print(f"Wrote report to {out_md}")

if __name__ == '__main__':
    main()