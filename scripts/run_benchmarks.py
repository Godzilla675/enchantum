#!/usr/bin/env python3
"""
Enchantum Benchmark Orchestration Script

This script runs all benchmark executables, parses their CSV output,
combines results, and generates speedup analysis relative to NAIVE policy.
"""

import os
import sys
import subprocess
import csv
import re
from pathlib import Path
from typing import Dict, List, Tuple, Optional
import tempfile
import argparse


class BenchmarkResult:
    def __init__(self, enum_name: str, policy: str, workload: str, mean_ns: float, p90_ns: float):
        self.enum_name = enum_name
        self.policy = policy
        self.workload = workload
        self.mean_ns = mean_ns
        self.p90_ns = p90_ns


class BenchmarkRunner:
    def __init__(self, build_dir: str = "build/benchmarks", output_dir: str = "benchmarks/results"):
        self.build_dir = Path(build_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Policy mapping: executable suffix -> policy name
        self.policies = {
            "auto": "AUTO",
            "naive": "NAIVE", 
            "len_first": "LEN_FIRST"
        }
        
        self.results: List[BenchmarkResult] = []
        self.compiler_id: Optional[str] = None

    def find_executables(self) -> Dict[str, Path]:
        """Find all benchmark executables in the build directory."""
        executables = {}
        for policy_key in self.policies:
            exe_name = f"enchantum_bench_{policy_key}"
            exe_path = self.build_dir / exe_name
            if exe_path.exists() and exe_path.is_file():
                executables[policy_key] = exe_path
            else:
                print(f"Warning: {exe_path} not found")
        return executables

    def run_benchmark(self, executable: Path) -> Tuple[Optional[str], List[BenchmarkResult]]:
        """Run a single benchmark executable and parse its output."""
        print(f"Running {executable.name}...")
        
        try:
            result = subprocess.run([str(executable)], 
                                  capture_output=True, 
                                  text=True, 
                                  timeout=300)  # 5 minute timeout
            if result.returncode != 0:
                print(f"Error running {executable}: {result.stderr}")
                return None, []
                
            return self.parse_output(result.stdout)
            
        except subprocess.TimeoutExpired:
            print(f"Timeout running {executable}")
            return None, []
        except Exception as e:
            print(f"Exception running {executable}: {e}")
            return None, []

    def parse_output(self, output: str) -> Tuple[Optional[str], List[BenchmarkResult]]:
        """Parse benchmark output to extract compiler ID and CSV results."""
        lines = output.strip().split('\n')
        
        # Extract compiler ID
        compiler_id = None
        for line in lines:
            if line.startswith("COMPILER_ID:"):
                compiler_id = line.split(":", 1)[1].strip()
                break
        
        # Find CSV section
        csv_start = None
        for i, line in enumerate(lines):
            if line.strip() == "=== CSV RESULTS ===":
                csv_start = i + 1
                break
        
        if csv_start is None:
            print("Warning: No CSV results section found")
            return compiler_id, []
        
        # Parse CSV data
        results = []
        csv_lines = lines[csv_start:]
        
        if not csv_lines:
            return compiler_id, []
            
        # Skip header if present
        header_line = csv_lines[0]
        if header_line.startswith("enum,policy,workload"):
            csv_lines = csv_lines[1:]
        
        for line in csv_lines:
            if line.strip():
                try:
                    parts = line.split(',')
                    if len(parts) >= 5:
                        enum_name = parts[0].strip()
                        policy = parts[1].strip()
                        workload = parts[2].strip()
                        mean_ns = float(parts[3].strip())
                        p90_ns = float(parts[4].strip())
                        
                        results.append(BenchmarkResult(enum_name, policy, workload, mean_ns, p90_ns))
                except ValueError as e:
                    print(f"Warning: Failed to parse CSV line '{line}': {e}")
                    continue
        
        return compiler_id, results

    def run_all_benchmarks(self):
        """Run all available benchmark executables."""
        executables = self.find_executables()
        
        if not executables:
            print("No benchmark executables found!")
            return False
        
        for policy_key, exe_path in executables.items():
            compiler_id, results = self.run_benchmark(exe_path)
            
            if compiler_id and self.compiler_id is None:
                self.compiler_id = compiler_id
            elif compiler_id and self.compiler_id != compiler_id:
                print(f"Warning: Different compiler IDs detected: {self.compiler_id} vs {compiler_id}")
            
            self.results.extend(results)
            
            # Save individual CSV
            policy_name = self.policies[policy_key]
            csv_file = self.output_dir / f"raw_csv" / f"{self.compiler_id}_{policy_name}.csv"
            csv_file.parent.mkdir(parents=True, exist_ok=True)
            self.save_csv(results, csv_file)
        
        return len(self.results) > 0

    def save_csv(self, results: List[BenchmarkResult], filepath: Path):
        """Save results to CSV file."""
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['enum', 'policy', 'workload', 'mean_ns', 'p90_ns'])
            for result in results:
                writer.writerow([result.enum_name, result.policy, result.workload, 
                               result.mean_ns, result.p90_ns])

    def save_combined_csv(self):
        """Save all results to a combined CSV file."""
        if not self.compiler_id:
            print("No compiler ID available")
            return
            
        filepath = self.output_dir / f"combined_{self.compiler_id}.csv"
        self.save_csv(self.results, filepath)
        print(f"Combined results saved to {filepath}")

    def calculate_speedups(self) -> Dict[Tuple[str, str], Dict[str, float]]:
        """Calculate speedup relative to NAIVE policy for each enum+workload combination."""
        # Group results by (enum, workload)
        grouped = {}
        for result in self.results:
            key = (result.enum_name, result.workload)
            if key not in grouped:
                grouped[key] = {}
            grouped[key][result.policy] = result
        
        # Calculate speedups
        speedups = {}
        for (enum_name, workload), policies in grouped.items():
            if "NAIVE" not in policies:
                continue
                
            naive_result = policies["NAIVE"]
            speedup_data = {}
            
            for policy, result in policies.items():
                if policy == "NAIVE":
                    speedup_mean = 1.0
                    speedup_p90 = 1.0
                else:
                    # Speedup = naive_time / policy_time (higher is better)
                    speedup_mean = naive_result.mean_ns / result.mean_ns if result.mean_ns > 0 else 0
                    speedup_p90 = naive_result.p90_ns / result.p90_ns if result.p90_ns > 0 else 0
                
                speedup_data[policy] = {
                    'speedup_mean': speedup_mean,
                    'speedup_p90': speedup_p90,
                    'mean_ns': result.mean_ns,
                    'p90_ns': result.p90_ns
                }
            
            speedups[(enum_name, workload)] = speedup_data
        
        return speedups

    def generate_summary_markdown(self):
        """Generate markdown summary with speedup analysis."""
        if not self.compiler_id:
            print("No compiler ID available")
            return
            
        speedups = self.calculate_speedups()
        
        filepath = self.output_dir / f"summary_{self.compiler_id}.md"
        
        with open(filepath, 'w') as f:
            f.write(f"# Benchmark Results Summary - {self.compiler_id}\n\n")
            f.write("## Speedup Analysis (Relative to NAIVE)\n\n")
            f.write("| Enum | Workload | Policy | Mean Speedup | P90 Speedup | Mean (ns) | P90 (ns) |\n")
            f.write("|------|----------|---------|--------------|-------------|-----------|----------|\n")
            
            # Sort for consistent output
            for (enum_name, workload) in sorted(speedups.keys()):
                policy_data = speedups[(enum_name, workload)]
                
                for policy in ["NAIVE", "AUTO", "LEN_FIRST"]:
                    if policy in policy_data:
                        data = policy_data[policy]
                        f.write(f"| {enum_name} | {workload} | {policy} | "
                               f"{data['speedup_mean']:.2f}x | {data['speedup_p90']:.2f}x | "
                               f"{data['mean_ns']:.2f} | {data['p90_ns']:.2f} |\n")
            
            f.write("\n## Performance Summary\n\n")
            
            # Calculate average speedups across all benchmarks
            all_speedups_mean = {"AUTO": [], "LEN_FIRST": []}
            all_speedups_p90 = {"AUTO": [], "LEN_FIRST": []}
            
            for policy_data in speedups.values():
                for policy in ["AUTO", "LEN_FIRST"]:
                    if policy in policy_data:
                        all_speedups_mean[policy].append(policy_data[policy]['speedup_mean'])
                        all_speedups_p90[policy].append(policy_data[policy]['speedup_p90'])
            
            f.write("**Average Speedups (vs NAIVE):**\n\n")
            for policy in ["AUTO", "LEN_FIRST"]:
                if all_speedups_mean[policy]:
                    avg_mean = sum(all_speedups_mean[policy]) / len(all_speedups_mean[policy])
                    avg_p90 = sum(all_speedups_p90[policy]) / len(all_speedups_p90[policy])
                    f.write(f"- {policy}: {avg_mean:.2f}x (mean), {avg_p90:.2f}x (p90)\n")
        
        print(f"Summary saved to {filepath}")


def main():
    parser = argparse.ArgumentParser(description="Run Enchantum benchmarks and generate reports")
    parser.add_argument("--build-dir", default="build/benchmarks", 
                       help="Directory containing benchmark executables")
    parser.add_argument("--output-dir", default="benchmarks/results",
                       help="Directory to save results")
    
    args = parser.parse_args()
    
    runner = BenchmarkRunner(args.build_dir, args.output_dir)
    
    if not runner.run_all_benchmarks():
        print("Failed to run benchmarks")
        return 1
    
    print(f"Collected {len(runner.results)} benchmark results")
    
    runner.save_combined_csv()
    runner.generate_summary_markdown()
    
    print("Benchmark analysis complete!")
    return 0


if __name__ == "__main__":
    sys.exit(main())