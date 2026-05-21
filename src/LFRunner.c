#include "LFRunner.h"
#include "LFTime.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int LFIsCrashSignal(int signal_number)
{
    if (signal_number == SIGSEGV) {
        return 1;
    }

    if (signal_number == SIGBUS) {
        return 1;
    }

    if (signal_number == SIGABRT) {
        return 1;
    }

    if (signal_number == SIGILL) {
        return 1;
    }

    if (signal_number == SIGFPE) {
        return 1;
    }

    return 0;
}

static int LFQuietChildOutput(void)
{
    int null_fd;

    null_fd = open("/dev/null", O_WRONLY);

    if (null_fd < 0) {
        return -1;
    }

    if (dup2(null_fd, STDOUT_FILENO) < 0) {
        close(null_fd);
        return -1;
    }

    if (dup2(null_fd, STDERR_FILENO) < 0) {
        close(null_fd);
        return -1;
    }

    if (null_fd > STDERR_FILENO) {
        close(null_fd);
    }

    return 0;
}

static void LFSetExitResult(int exit_code, LFRunResult *result)
{
    result->exit_code = exit_code;

    if (exit_code == 0) {
        result->kind = LF_RESULT_OK;
        return;
    }

    if (exit_code == 1) {
        result->kind = LF_RESULT_DOMAIN_REJECT;
        return;
    }

    if (exit_code == 2) {
        result->kind = LF_RESULT_USAGE;
        return;
    }

    if (exit_code == 127) {
        result->kind = LF_RESULT_EXEC_ERROR;
        return;
    }

    result->kind = LF_RESULT_EXIT_ERROR;
}

int LFRunTarget(
    const char *target_path,
    const char *input_path,
    int timeout_seconds,
    LFOutputMode output_mode,
    LFRunResult *result
)
{
    pid_t pid;
    int status;
    int wait_result;
    double start_ms;
    double now_ms;

    if (target_path == 0 || input_path == 0 || result == 0) {
        return -1;
    }

    memset(result, 0, sizeof(*result));
    result->kind = LF_RESULT_EXEC_ERROR;
    result->exit_code = -1;
    result->signal_number = 0;
    result->timed_out = 0;
    result->elapsed_ms = 0.0;

    start_ms = LFNowMilliseconds();

    pid = fork();

    if (pid < 0) {
        result->elapsed_ms = LFNowMilliseconds() - start_ms;
        result->kind = LF_RESULT_EXEC_ERROR;
        return -1;
    }

    if (pid == 0) {
        if (output_mode == LF_OUTPUT_QUIET) {
            if (LFQuietChildOutput() != 0) {
                _exit(127);
            }
        }

        execlp(target_path, target_path, input_path, (char *)0);
        _exit(127);
    }

    for (;;) {
        wait_result = waitpid(pid, &status, WNOHANG);

        if (wait_result == pid) {
            break;
        }

        if (wait_result < 0) {
            if (errno == EINTR) {
                continue;
            }

            result->elapsed_ms = LFNowMilliseconds() - start_ms;
            result->kind = LF_RESULT_EXEC_ERROR;
            return -1;
        }

        now_ms = LFNowMilliseconds();

        if (timeout_seconds > 0) {
            if ((now_ms - start_ms) >= ((double)timeout_seconds * 1000.0)) {
                result->timed_out = 1;
                kill(pid, SIGKILL);
                waitpid(pid, &status, 0);
                result->elapsed_ms = LFNowMilliseconds() - start_ms;
                result->kind = LF_RESULT_TIMEOUT;
                result->signal_number = SIGKILL;
                return 0;
            }
        }

        usleep(10000);
    }

    result->elapsed_ms = LFNowMilliseconds() - start_ms;

    if (WIFEXITED(status)) {
        LFSetExitResult(WEXITSTATUS(status), result);
        return 0;
    }

    if (WIFSIGNALED(status)) {
        result->signal_number = WTERMSIG(status);

        if (LFIsCrashSignal(result->signal_number)) {
            result->kind = LF_RESULT_CRASH;
        } else {
            result->kind = LF_RESULT_SIGNAL;
        }

        return 0;
    }

    result->kind = LF_RESULT_EXEC_ERROR;
    return 0;
}
