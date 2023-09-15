import time

def thread_lock_duration(debugger, command, result, internal_dict):
    thread = debugger.GetSelectedTarget().GetProcess().GetSelectedThread()
    start_time = time.time()
    debugger.GetCommandInterpreter().HandleCommand("thread continue", result)
    end_time = time.time()
    duration_ms = (end_time - start_time) * 1000  # Convert to milliseconds

    if duration_ms > 100:
        print(f"Thread {thread.GetThreadID()} locked for {duration_ms} ms")


