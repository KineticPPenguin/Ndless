#include <nucleus.h>
#include <syscall-list.h>
#include <syscall-addrs.h>
#include <syscall.h>

extern "C" void ut_read_os_version_index();

int main()
{
	ut_read_os_version_index();

	const char *res_path = "/documents/ndless/ndless_resources.tns";
	NUC_FILE *res_fp = syscall_local<e_fopen, NUC_FILE*>(res_path, "rb");
	char *res_argv = nullptr;

        if (!res_fp)
	{
		syscall_local<e_puts, int>("ndless_resources not found.");
		return 1;
	}

	struct nuc_stat res_stat;
        syscall_local<e_stat, int>(res_path, &res_stat);
        char *core = syscall_local<e_malloc, char*>(res_stat.st_size);
        syscall_local<e_fread, int>(core, res_stat.st_size, 1, res_fp);
        syscall_local<e_fclose, int>(res_fp);

        asm volatile(	"0: mrc p15, 0, r15, c7, c10, 3 @ test and clean DCache \n"
			" bne 0b \n"
			" mov r0, #0 \n"
			" mcr p15, 0, r0, c7, c7, 0 @ invalidate ICache and DCache \n" ::: "r0");

        ((int (*)(int argc, void* argv))(core + sizeof("PRG")))(1, &res_argv);
	return 0;
}