#!/usr/bin/env python3
import subprocess
import time
import shlex
import sys
import os
from typing import List, Tuple, Dict, Any
import statistics

def time_command(command: str, description: str = "", skip_if_exists: List[str] = None) -> Tuple[float, bool]:
    if skip_if_exists:
        all_exist = True
        for filepath in skip_if_exists:
            if not os.path.exists(filepath):
                all_exist = False
                break
        if all_exist:
            print(f"Skipping (files exist): {description}")
            return 0.0, True
    
    print(f"Executing: {description}")
    print(f"   Command: {command[:100]}..." if len(command) > 100 else f"   Command: {command}")
    
    try:
        start_time = time.perf_counter()
        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True,
            check=False
        )
        elapsed_time = time.perf_counter() - start_time
        
        if result.returncode != 0:
            print(f"   Error (code: {result.returncode})")
            if result.stderr:
                print(f"   Error message: {result.stderr[:200]}")
            return elapsed_time, False
        
        print(f"   Success in {elapsed_time:.3f} seconds")
        return elapsed_time, True
        
    except Exception as e:
        print(f"   Exception: {e}")
        return 0.0, False

def analyze_target(commands: List[Dict], target_name: str, iterations: int = 3, clean: bool = False) -> Dict[str, Any]:
    print(f"\n{'='*60}")
    print(f"Analyzing target: {target_name}")
    print(f"{'='*60}")
    
    results = {
        'target_name': target_name,
        'commands': [cmd['cmd'] for cmd in commands],
        'command_metadata': commands,
        'total_times': [],
        'success': False,
        'command_times': [],
        'num_commands': len(commands),
        'categories': {'scan': 0, 'compile': 0, 'link': 0}
    }
    
    for cmd_info in commands:
        category = cmd_info.get('category', 'unknown')
        if category in results['categories']:
            results['categories'][category] += 1
    
    if clean:
        print("\nCleaning build artifacts...")
        clean_dirs = ['06/6.01', '06/6.02']
        for dir_path in clean_dirs:
            if os.path.exists(dir_path):
                for root, dirs, files in os.walk(dir_path):
                    for file in files:
                        if file.endswith(('.o', '.d', '.ddi', '.tmp', '.modmap')):
                            try:
                                os.remove(os.path.join(root, file))
                            except:
                                pass
        print("Cleaning completed")
    
    for iteration in range(iterations):
        print(f"\n--- Iteration {iteration + 1}/{iterations} ---")
        
        iteration_times = []
        iteration_details = []
        all_success = True
        
        for i, cmd_info in enumerate(commands, 1):
            cmd = cmd_info['cmd']
            desc = cmd_info.get('description', f"Command {i}/{len(commands)}")
            category = cmd_info.get('category', 'unknown')
            skip_files = cmd_info.get('skip_if_exists', None)
            
            elapsed, success = time_command(cmd, desc, skip_files)
            iteration_times.append(elapsed)
            iteration_details.append({
                'time': elapsed,
                'category': category,
                'success': success
            })
            
            if not success:
                all_success = False
                break
        
        if all_success and len(iteration_times) == len(commands):
            total_time = sum(iteration_times)
            results['total_times'].append(total_time)
            results['command_times'].append(iteration_details)
            print(f"Total iteration time: {total_time:.3f} seconds")
    
    if results['total_times']:
        results['success'] = True
        avg_total = statistics.mean(results['total_times'])
        min_total = min(results['total_times'])
        max_total = max(results['total_times'])
        std_total = statistics.stdev(results['total_times']) if len(results['total_times']) > 1 else 0
        
        print(f"\nStatistics for '{target_name}':")
        print(f"   Average build time: {avg_total:.3f} sec")
        print(f"   Minimum time:      {min_total:.3f} sec")
        print(f"   Maximum time:      {max_total:.3f} sec")
        if std_total > 0:
            print(f"   Standard deviation: {std_total:.3f} sec")
        print(f"   Total commands: {len(commands)}")
        print(f"     • Dependency scanning: {results['categories']['scan']}")
        print(f"     • Compilation: {results['categories']['compile']}")
        print(f"     • Linking: {results['categories']['link']}")
        
        results['avg_total'] = avg_total
        results['min_total'] = min_total
        results['max_total'] = max_total
        results['std_total'] = std_total
        
        if results['command_times']:
            scan_times = []
            compile_times = []
            link_times = []
            
            for iteration in results['command_times']:
                for i, cmd_detail in enumerate(iteration):
                    category = commands[i].get('category', 'unknown')
                    if category == 'scan':
                        scan_times.append(cmd_detail['time'])
                    elif category == 'compile':
                        compile_times.append(cmd_detail['time'])
                    elif category == 'link':
                        link_times.append(cmd_detail['time'])
            
            if scan_times:
                avg_scan = statistics.mean(scan_times)
                print(f"\nAverage time by category:")
                print(f"   Dependency scanning: {avg_scan:.3f} sec")
                results['avg_scan'] = avg_scan
            
            if compile_times:
                avg_compile = statistics.mean(compile_times)
                print(f"   Compilation: {avg_compile:.3f} sec")
                results['avg_compile'] = avg_compile
            
            if link_times:
                avg_link = statistics.mean(link_times)
                print(f"   Linking: {avg_link:.3f} sec")
                results['avg_link'] = avg_link
    
    return results

def compare_targets(target1_results: Dict[str, Any], target2_results: Dict[str, Any]):
    print(f"\n{'='*60}")
    print("TARGET COMPARISON")
    print(f"{'='*60}")
    
    if not target1_results.get('success', False):
        print("First target was not successfully built")
        return
    
    if not target2_results.get('success', False):
        print("Second target was not successfully built")
        return
    
    t1_avg = target1_results.get('avg_total', 0)
    t2_avg = target2_results.get('avg_total', 0)
    
    print(f"\nBuild times:")
    print(f"   Target 1 (no modules): {t1_avg:.3f} sec")
    print(f"   Target 2 (with modules): {t2_avg:.3f} sec")
    
    if t1_avg < t2_avg:
        diff = t2_avg - t1_avg
        percent = (diff / t1_avg) * 100 if t1_avg > 0 else 0
        print(f"\nTarget 1 is faster by {diff:.3f} sec ({percent:.1f}%)")
    else:
        diff = t1_avg - t2_avg
        percent = (diff / t2_avg) * 100 if t2_avg > 0 else 0
        print(f"\nTarget 2 is faster by {diff:.3f} sec ({percent:.1f}%)")
    
    print(f"\nBuild characteristics:")
    print(f"   Target 1: {target1_results.get('num_commands', 0)} commands")
    print(f"     • Compilation: {target1_results.get('categories', {}).get('compile', 0)}")
    print(f"     • Linking: {target1_results.get('categories', {}).get('link', 0)}")
    
    print(f"   Target 2: {target2_results.get('num_commands', 0)} commands")
    print(f"     • Dependency scanning: {target2_results.get('categories', {}).get('scan', 0)}")
    print(f"     • Compilation: {target2_results.get('categories', {}).get('compile', 0)}")
    print(f"     • Linking: {target2_results.get('categories', {}).get('link', 0)}")
    
    commands2 = target2_results.get('command_metadata', [])
    if commands2 and len(commands2) > 0:
        compile_cmd = None
        for cmd_info in commands2:
            if cmd_info.get('category') == 'compile':
                compile_cmd = cmd_info['cmd']
                break
        
        if compile_cmd:
            if "-fmodules" in compile_cmd:
                print(f"Target 2 uses C++ modules (-fmodules flag)")
            if "stdlib=libc++" in compile_cmd:
                print(f"Target 2 uses libc++ instead of libstdc++")
            if "clang-scan-deps" in commands2[0]['cmd']:
                print(f"Target 2 uses dependency scanning (clang-scan-deps)")
    
    if 'avg_compile' in target1_results and 'avg_compile' in target2_results:
        print(f"\nCompilation time comparison:")
        print(f"   Target 1: {target1_results['avg_compile']:.3f} sec")
        print(f"   Target 2: {target2_results['avg_compile']:.3f} sec")
    
    if 'avg_scan' in target2_results:
        print(f"   Target 2 (scanning): {target2_results['avg_scan']:.3f} sec")

def main():
    target1_commands = [
        {
            'cmd': "/usr/bin/clang++-20 -std=gnu++23 -MD -MT 06/6.01/CMakeFiles/rational_06_01.dir/main.cpp.o -MF 06/6.01/CMakeFiles/rational_06_01.dir/main.cpp.o.d -o 06/6.01/CMakeFiles/rational_06_01.dir/main.cpp.o -c /home/ac-sc/study/cpp_mipt/06/6.01/main.cpp",
            'description': "Compile main.cpp",
            'category': 'compile'
        },
        {
            'cmd': "/usr/bin/clang++-20 -std=gnu++23 -MD -MT 06/6.01/CMakeFiles/rational_06_01.dir/rational.cpp.o -MF 06/6.01/CMakeFiles/rational_06_01.dir/rational.cpp.o.d -o 06/6.01/CMakeFiles/rational_06_01.dir/rational.cpp.o -c /home/ac-sc/study/cpp_mipt/06/6.01/rational.cpp",
            'description': "Compile rational.cpp",
            'category': 'compile'
        },
        {
            'cmd': "/usr/bin/clang++-20 -Xlinker --dependency-file -Xlinker 06/6.01/CMakeFiles/rational_06_01.dir/link.d 06/6.01/CMakeFiles/rational_06_01.dir/main.cpp.o 06/6.01/CMakeFiles/rational_06_01.dir/rational.cpp.o -o 06/6.01/rational_06_01",
            'description': "Link executable",
            'category': 'link'
        }
    ]
    
    target2_commands = [
        {
            'cmd': '"/usr/bin/clang-scan-deps-20" -format=p1689 -- /usr/bin/clang++-20 -fmodules -stdlib=libc++ -std=gnu++23 -x c++ /home/ac-sc/study/cpp_mipt/06/6.02/rational.cppm -c -o 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o -resource-dir "/usr/lib/llvm-20/lib/clang/20" -MT 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.ddi -MD -MF 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.ddi.d > 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.ddi.tmp && mv 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.ddi.tmp 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.ddi',
            'description': "Dependency scanning for rational.cppm",
            'category': 'scan',
        },
        {
            'cmd': '"/usr/bin/clang-scan-deps-20" -format=p1689 -- /usr/bin/clang++-20 -fmodules -stdlib=libc++ -std=gnu++23 -x c++ /home/ac-sc/study/cpp_mipt/06/6.02/rational.cpp -c -o 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o -resource-dir "/usr/lib/llvm-20/lib/clang/20" -MT 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.ddi -MD -MF 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.ddi.d > 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.ddi.tmp && mv 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.ddi.tmp 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.ddi',
            'description': "Dependency scanning for rational.cpp",
            'category': 'scan',
        },
        {
            'cmd': '"/usr/bin/clang-scan-deps-20" -format=p1689 -- /usr/bin/clang++-20 -fmodules -stdlib=libc++ -std=gnu++23 -x c++ /home/ac-sc/study/cpp_mipt/06/6.02/main.cpp -c -o 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o -resource-dir "/usr/lib/llvm-20/lib/clang/20" -MT 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.ddi -MD -MF 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.ddi.d > 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.ddi.tmp && mv 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.ddi.tmp 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.ddi',
            'description': "Dependency scanning for main.cpp",
            'category': 'scan',
        },
        {
            'cmd': "/usr/bin/clang++-20 -fmodules -stdlib=libc++ -std=gnu++23 -MD -MT 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o -MF 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.d @06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o.modmap -o 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o -c /home/ac-sc/study/cpp_mipt/06/6.02/rational.cppm",
            'description': "Compile rational.cppm module",
            'category': 'compile',
        },
        {
            'cmd': "/usr/bin/clang++-20 -fmodules -stdlib=libc++ -std=gnu++23 -MD -MT 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o -MF 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.d @06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o.modmap -o 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o -c /home/ac-sc/study/cpp_mipt/06/6.02/rational.cpp",
            'description': "Compile rational.cpp",
            'category': 'compile',
        },
        {
            'cmd': "/usr/bin/clang++-20 -fmodules -stdlib=libc++ -std=gnu++23 -MD -MT 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o -MF 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.d @06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o.modmap -o 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o -c /home/ac-sc/study/cpp_mipt/06/6.02/main.cpp",
            'description': "Compile main.cpp",
            'category': 'compile',
        },
        {
            'cmd': "/usr/bin/clang++-20 -fmodules -stdlib=libc++ -Xlinker --dependency-file -Xlinker 06/6.02/CMakeFiles/rational_06_02.dir/link.d 06/6.02/CMakeFiles/rational_06_02.dir/main.cpp.o 06/6.02/CMakeFiles/rational_06_02.dir/rational.cpp.o 06/6.02/CMakeFiles/rational_06_02.dir/rational.cppm.o -o 06/6.02/rational_06_02",
            'description': "Link executable",
            'category': 'link',
        }
    ]
    
    iterations = 3
    clean = False
    
    for arg in sys.argv[1:]:
        if arg.isdigit():
            iterations = int(arg)
            if iterations < 1:
                print("Number of iterations must be positive. Using 3.")
                iterations = 3
        elif arg == "--clean":
            clean = True
        elif arg == "--help" or arg == "-h":
            print("Usage: python compare_builds.py [iterations] [--clean]")
            print("  iterations: number of runs for each target (default 3)")
            print("  --clean: clean build artifacts before execution")
            return
    
    print(f"Starting build comparison")
    print(f"Number of iterations per target: {iterations}")
    if clean:
        print("Build artifacts will be cleaned before execution")
    
    results1 = analyze_target(target1_commands, "rational_06_01 (no modules)", iterations, clean)
    results2 = analyze_target(target2_commands, "rational_06_02 (with C++20 modules)", iterations, clean)
    
    compare_targets(results1, results2)


if __name__ == "__main__":
    main()
