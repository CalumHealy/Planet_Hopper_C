import subprocess
import threading
import sys
import math

if len(sys.argv) < 2:
    print("You must specify an executable path!\ne.g. python benchmark.py ./out.exe")
    sys.exit(1)
EXE_PATH = sys.argv[1]

TOTAL_RUNS = 5000
if len(sys.argv) > 2:
    TOTAL_RUNS = int(sys.argv[2])
THREADS = 32

def execute_one(seed: int) -> (int, bool):
    command = [EXE_PATH, str(seed)]
    try:
        result = subprocess.run(command, capture_output=True, text=True)
        success = "Congratulations" in result.stdout.strip().split('\n')[-1]
        hops = result.returncode
        if success:
            print(f"Ran with seed {seed}, {hops} hops")
            return (hops, False)
        else:
            print(f"Ran with seed {seed} [FAIL]")
            return (0, True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command with seed {seed}: {e}")
        sys.exit(1)

def execute_chunk(start: int, total_hops: list(), total_fails: list()):
    chunk_hops = 0
    chunk_fails = 0
    for i in range(math.floor(TOTAL_RUNS / THREADS)):
        hops, fail = execute_one(start + i)
        chunk_hops += hops
        if fail:
            chunk_fails += 1
    total_hops[0] += chunk_hops
    total_fails[0] += chunk_fails

hops = [0]
fails = [0]

threads = []
for i in range(THREADS):
    thread = threading.Thread(target=execute_chunk, args=(i * math.floor(TOTAL_RUNS / THREADS), hops, fails))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()

success_count = TOTAL_RUNS - fails[0]
success_rate = success_count / TOTAL_RUNS
fail_rate = fails[0] / TOTAL_RUNS
average_hops = hops[0] / success_count
print(f"Successes: {success_count}\nFailures: {fails[0]}\nSuccess Rate: {success_rate * 100:.2f}%\nFail Rate: {fail_rate * 100:.2f}%")
print(f"Average hops: {average_hops:.1f} ")