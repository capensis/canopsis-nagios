#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <iniparser.h>

#define DB_NAME "canopsis.cache"
#define DB_SIZE 500

static unsigned int
file_exists (const char *file)
{
    int status;
    struct stat s;
    status = stat (file, &s);
    if (status == -1)
        fprintf (stderr, "stat: %s\n", strerror(errno));
    return (status == 0);
}

static int
create_empty_file (const char *file)
{
    int fd;
    int flags = O_CREAT|O_WRONLY|O_TRUNC;
    fd = open (file, flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (fd < 0) {
        fprintf (stderr, "%s: %s\n", file, strerror (errno));
        return -1;
    }
    close (fd);
    return 0;
}

static int compare (const void * a, const void * b)
{
    /* The pointers point to offsets into "array", so we need to
       dereference them to get at the strings. */

   return strcmp (*(const char **) a, *(const char **) b);
}

int main (int args, char **argv) {
    dictionary *ini;
    fprintf (stdout, "Testing the ini lib\n");
    if (!file_exists (DB_NAME)) {
        int r = create_empty_file (DB_NAME);
        if (r < 0)
            return 1;
        int fd[2];
        pipe (fd);
        pid_t pid = fork ();
        if (pid == 0) {
            FILE *tmp;
            int i = 1;
            tmp = fdopen (fd[1], "w");
            fprintf (tmp, "[cache]\n");
/*
            fprintf (tmp, "size=0\n");
            for (; i <= DB_SIZE; i++)
                fprintf (tmp, "data_%d=\"\"\n", i);
*/
            fclose (tmp);
            close (fd[1]);
            close (fd[0]);
            exit (0);
        }
        FILE *input;
        close (fd[1]);
        input = fdopen (fd[0], "r");
        ini = iniparser_load_fd (input);
        fclose (input);
        close (fd[0]);
    } else {
        ini = iniparser_load (DB_NAME);
    }

    if (ini==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", DB_NAME);
        return -1;
    }

    iniparser_set (ini, "cache:size", "2");

    iniparser_set (ini, "cache:data_1", ";0;32000 'tbs_pgmwork_alloc'=38MB;;;0;32000 'tbs_pgmlarge_usage_pct'=0.02%;90;98 'tbs_pgmlarge_usage'=6MB;28800;31360;0;32000 'tbs_pgmlarge_alloc'= 7MB;;;0;32000 'tbs_pgmapp_usage_pct'=0.07%;90;98 'tbs_pgmapp_usage'=21MB;28800;31360;0;32000 'tbs_pgmapp_alloc'=23MB;;;0;32000 'tbs_pcwork_usage_pct'=0.00%;90;98 'tbs_pcw ork_usage'=0MB;28800;31360;0;32000 'tbs_pcwork_alloc'=5MB;;;0;32000 'tbs_pclarge_usage_pct'=5.14%;90;98 'tbs_pclarge_usage'=1644MB;28800;31360;0;32000 'tbs_pclarge_alloc' =1695MB;;;0;32000 'tbs_pcarch_usage_pct'=0.00%;90;98 'tbs_pcarch_usage'=0MB;28800;31360;0;32000 'tbs_pcarch_alloc'=5MB;;;0;32000 'tbs_pcapp_usage_pct'=1.54%;90;98 'tbs_pc app_usage'=493MB;28800;31360;0;32000 'tbs_pcapp_alloc'=637MB;;;0;32000 'tbs_pbwork_usage_pct'=0.00%;90;98 'tbs_pbwork_usage'=0MB;28800;31360;0;32000 'tbs_pbwork_alloc'=5M B;;;0;32000 'tbs_pblarge_usage_pct'=0.00%;90;98 'tbs_pblarge_usage'=0MB;28800;31360;0;32000 'tbs_pblarge_alloc'=5MB;;;0;32000 'tbs_pbimage_usage_pct'=0.00%;90;98 'tbs_pbi mage_usage'=0MB;28800;31360;0;32000 'tbs_pbimage_alloc'=5MB;;;0;32000 'tbs_pbapp_usage_pct'=0.00%;90;98 'tbs_pbapp_usage'=0MB;28800;31360;0;32000 'tbs_pbapp_alloc'=5MB;;; 0;32000 'tbs_omwork_usage_pct'=1.06%;90;98 'tbs_omwork_usage'=338MB;28800;31360;0;32000 'tbs_omwork_alloc'=355MB;;;0;32000 'tbs_omlarge_usage_pct'=0.10%;90;98 'tbs_omlarg e_usage'=31MB;28800;31360;0;32000 'tbs_omlarge_alloc'=38MB;;;0;32000 'tbs_omapp_usage_pct'=0.13%;90;98 'tbs_omapp_usage'=40MB;28800;31360;0;32000 'tbs_omapp_alloc'=116MB; ;;0;32000 'tbs_oeapp_usage_pct'=0.00%;90;98 'tbs_oeapp_usage'=0MB;28800;31360;0;32000 'tbs_oeapp_alloc'=5MB;;;0;32000 'tbs_mpwork_usage_pct'=0.00%;90;98 'tbs_mpwork_usage '=0MB;28800;31360;0;32000 'tbs_mpwork_alloc'=5MB;;;0;32000 'tbs_mplarge_usage_pct'=0.00%;90;98 'tbs_mplarge_usage'=0MB;28800;31360;0;32000 'tbs_mplarge_alloc'=5MB;;;0;320 00 'tbs_mpapp_usage_pct'=0.03%;90;98 'tbs_mpapp_usage'=9MB;28800;31360;0;32000 'tbs_mpapp_alloc'=10MB;;;0;32000 'tbs_mglarge_usage_pct'=0.02%;90;98 'tbs_mglarge_usage'=4M B;28800;31360;0;32000 'tbs_mglarge_alloc'=6MB;;;0;32000 'tbs_mgapp_usage_pct'=0.30%;90;98 'tbs_mgapp_usage'=95MB;28800;31360;0;32000 'tbs_mgapp_alloc'=100MB;;;0;32000 'tb s_lclarge_usage_pct'=19.57%;90;98 'tbs_lclarge_usage'=6261MB;28800;31360;0;32000 'tbs_lclarge_alloc'=6644MB;;;0;32000 'tbs_lcapp_usage_pct'=2.13%;90;98 'tbs_lcapp_usage'= 681MB;28800;31360;0;32000 'tbs_lcapp_alloc'=792MB;;;0;32000 'tbs_inwork_usage_pct'=0.73%;90;98 'tbs_inwork_usage'=234MB;28800;31360;0;32000 'tbs_inwork_alloc'=246MB;;;0;3 2000 'tbs_inlarge_usage_pct'=12.95%;90;98 'tbs_inlarge_usage'=4144MB;28800;31360;0;32000 'tbs_inlarge_alloc'=4352MB;;;0;32000 'tbs_inarch_usage_pct'=0.00%;90;98 'tbs_inar ch_usage'=1MB;28800;31360;0;32000 'tbs_inarch_alloc'=2MB;;;0;32000 'tbs_inapp_usage_pct'=2.84%;90;98 'tbs_inapp_usage'=908MB;28800;31360;0;32000 'tbs_inapp_alloc'=954MB;; ;0;32000 'tbs_htapp_usage_pct'=0.00%;90;98 'tbs_htapp_usage'=0MB;28800;31360;0;32000 'tbs_htapp_alloc'=2MB;;;0;32000 'tbs_hrlarge_usage_pct'=0.07%;90;98 'tbs_hrlarge_usag e'=23MB;28800;31360;0;32000 'tbs_hrlarge_alloc'=26MB;;;0;32000 'tbs_hrapp_usage_pct'=0.07%;90;98 'tbs_hrapp_usage'=23MB;28800;31360;0;32000 'tbs_hrapp_alloc'=32MB;;;0;320 00 'tbs_gmapp_usage_pct'=0.15%;90;98 'tbs_gmapp_usage'=47MB;28800;31360;0;32000 'tbs_gmapp_alloc'=50MB;;;0;32000 'tbs_glwork_usage_pct'=0.28%;90;98 'tbs_glwork_usage'=90M B;28800;31360;0;32000 'tbs_glwork_alloc'=105MB;;;0;32000 'tbs_gllarge_usage_pct'=3.75%;90;98 'tbs_gllarge_usage'=1198MB;28800;31360;0;32000 'tbs_gllarge_alloc'=1201MB;;;0 ;32000 'tbs_glarch_usage_pct'=0.02%;90;98 'tbs_glarch_usage'=5MB;28800;31360;0;32000 'tbs_glarch_alloc'=7MB;;;0;32000 'tbs_glapp_usage_pct'=1.37%;90;98 'tbs_glapp_usage'= 438MB;28800;31360;0;32000 'tbs_glapp_alloc'=612MB;;;0;32000 'tbs_fswork_usage_pct'=0.23%;90;98 'tbs_fswork_usage'=72MB;28800;31360;0;32000 'tbs_fswork_alloc'=76MB;;;0;320 00 'tbs_fslarge_usage_pct'=2.83%;90;98 'tbs_fslarge_usage'=904MB;28800;31360;0;32000 'tbs_fslarge_alloc'=924MB;;;0;32000 'tbs_fsapp1_usage_pct'=0.00%;90;98 'tbs_fsapp1_us age'=1MB;28800;31360;0;32000 'tbs_fsapp1_alloc'=2MB;;;0;32000 'tbs_fsapp_usage_pct'=9.64%;90;98 'tbs_fsapp_usage'=3086MB;28800;31360;0;32000 'tbs_fsapp_alloc'=3163MB;;;0; 32000 'tbs_fowork_usage_pct'=0.00%;90;98 'tbs_fowork_usage'=0MB;28800;31360;0;32000 'tbs_fowork_alloc'=5MB;;;0;32000 'tbs_folarge_usage_pct'=0.02%;90;98 'tbs_folarge_usag e'=5MB;28800;31360;0;32000 'tbs_folarge_alloc'=6MB;;;0;32000 'tbs_foimage_usage_pct'=0.00%;90;98 'tbs_foimage_usage'=0MB;28800;31360;0;32000 'tbs_foimage_alloc'=5MB;;;0;3 2000 'tbs_foapp_usage_pct'=0.03%;90;98 'tbs_foapp_usage'=8MB;28800;31360;0;32000 'tbs_foapp_alloc'=10MB;;;0;32000 'tbs_filarge_usage_pct'=0.00%;90;98 'tbs_filarge_usage'= 0MB;28800;31360;0;32000 'tbs_filarge_alloc'=5MB;;;0;32000 'tbs_fiapp_usage_pct'=0.00%;90;98 'tbs_fiapp_usage'=0MB;28800;31360;0;32000 'tbs_fiapp_alloc'=5MB;;;0;32000 'tbs _fgwork_usage_pct'=0.00%;90;98 'tbs_fgwork_usage'=0MB;28800;31360;0;32000 'tbs_fgwork_alloc'=5MB;;;0;32000 'tbs_fgtemp_usage_pct'=0.00%;90;98 'tbs_fgtemp_usage'=0MB;28800 ;31360;0;32000 'tbs_fgtemp_alloc'=5MB;;;0;32000 'tbs_fglarge_usage_pct'=0.02%;90;98 'tbs_fglarge_usage'=4MB;28800;31360;0;32000 'tbs_fglarge_alloc'=6MB;;;0;32000 'tbs_fgi mage_usage_pct'=0.00%;90;98 'tbs_fgimage_usage'=0MB;28800;31360;0;32000 'tbs_fgimage_alloc'=5MB;;;0;32000 'tbs_fgarch_usage_pct'=0.00%;90;98 'tbs_fgarch_usage'=0MB;28800; 31360;0;32000 'tbs_fgarch_alloc'=5MB;;;0;32000 'tbs_fgapp_usage_pct'=0.01%;90;98 'tbs_fgapp_usage'=2MB;28800;31360;0;32000 'tbs_fgapp_alloc'=4MB;;;0;32000 'tbs_fawork_usa ge_pct'=0.00%;90;98 'tbs_fawork_usage'=0MB;28800;31360;0;32000 'tbs_fawork_alloc'=5MB;;;0;32000 'tbs_falarge_usage_pct'=0.00%;90;98 'tbs_falarge_usage'=0MB;28800;31360;0; 32000 'tbs_falarge_alloc'=5MB;;;0;32000 'tbs_faapp_usage_pct'=0.04%;90;98 'tbs_faapp_usage'=11MB;28800;31360;0;32000 'tbs_faapp_alloc'=13MB;;;0;32000 'tbs_exwork_usage_pc t'=0.35%;90;98 'tbs_exwork_usage'=111MB;28800;31360;0;32000 'tbs_exwork_alloc'=181MB;;;0;32000 'tbs_exlarge_usage_pct'=1.19%;90;98 'tbs_exlarge_usage'=379MB;28800;31360;0 ;32000 'tbs_exlarge_alloc'=398MB;;;0;32000 'tbs_exarch_usage_pct'=0.01%;90;98 'tbs_exarch_usage'=3MB;28800;31360;0;32000 'tbs_exarch_alloc'=4MB;;;0;32000 'tbs_exapp_usage_pct'=0.04%;90;98 'tbs_exapp_usage'=13MB;28800;31360;0;32000 'tbs_exapp_alloc'=15MB;;;0;32000 'tbs_eotplrg_usage_pct'=0.00%;90;98 'tbs_eotplrg_usage'=1MB;28800;31360;0;32000 'tbs_eotplrg_alloc'=2MB;;;0;32000 'tbs_eotpapp_usage_pct'=0.01%;90;98 'tbs_eotpapp_usage'=2MB;28800;31360;0;32000 'tbs_eotpapp_alloc'=4MB;;;0;32000 'tbs_eopplrg_usage_pct'=0.00%;90;98 'tbs_eopplrg_usage'=0MB;28800;31360;0;32000 'tbs_eopplrg_alloc'=5MB;;;0;32000 'tbs_eoppapp_usage_pct'=0.01%;90;98 'tbs_eoppapp_usage'=3MB;28800;31360;0;32000 'tbs_eoppapp_alloc'=5MB;;;0;32000 'tbs_eoltapp_usage_pct'=0.01%;90;98 'tbs_eoltapp_usage'=2MB;28800;31360;0;32000 'tbs_eoltapp_alloc'=4MB;;;0;32000 'tbs_eolarge_usage_pct'=0.00%;90;98 'tbs_eolarge_usage'=0MB;28800;31360;0;32000 'tbs_eolarge_alloc'=5MB;;;0;32000 'tbs_eoiuwrk_usage_pct'=0.00%;90;98 'tbs_eoiuwrk_usage'=0MB;28800;31360;0;32000 'tbs_eoiuwrk_alloc'=5MB;;;0;32000 'tbs_eoiulrg_usage_pct'=0.00%;90;98 'tbs_eoiulrg_usage'=0MB;28800;31360;0;32000 'tbs_eoiulrg_alloc'=5MB;;;0;32000 'tbs_eoiuapp_usage_pct'=1.80%;90;98 'tbs_eoiuapp_usage'=576MB;28800;31360;0;32000 'tbs_eoiuapp_alloc'=605MB;;;0;32000 'tbs_eoewwrk_usage_pct'=0.00%;90;98 'tbs_eoewwrk_usage'=0MB;28800;31360;0;32000 'tbs_eoewwrk_alloc'=5MB;;;0;32000 'tbs_eoewlrg_usage_pct'=0.00%;90;98 'tbs_eoewlrg_usage'=0MB;28800;31360;0;32000 'tbs_eoewlrg_alloc'=5MB;;;0;32000 'tbs_eoewapp_usage_pct'=0.02%;90;98 'tbs_eoewapp_usage'=5MB;28800;31360;0;32000 'tbs_eoewapp_alloc'=7MB;;;0;32000 'tbs_eoepwrk_usage_pct'=0.04%;90;98 'tbs_eoepwrk");

    iniparser_set (ini, "cache:data_2", "usage'=12MB;28800;31360;0;32000 'tbs_eoepwrk_alloc'=13MB;;;0;32000 'tbs_eoeplrg_usage_pct'=0.00%;90;98 'tbs_eoeplrg_usage'=0MB;28800;31360;0;32000 'tbs_eoeplrg_alloc'=5MB;;;0;32000 'tbs_eoepapp_usage_pct'=0.02%;90;98 'tbs_eoepapp_usage'=7MB;28800;31360;0;32000 'tbs_eoepapp_alloc'=8MB;;;0;32000 'tbs_eoeilrg_usage_pct'=0.00%;90;98 'tbs_eoeilrg_usage'=0MB;28800;31360;0;32000 'tbs_eoeilrg_alloc'=5MB;;;0;32000 'tbs_eoeiapp_usage_pct'=0.04%;90;98 'tbs_eoeiapp_usage'=13MB;28800;31360;0;32000 'tbs_eoeiapp_alloc'=15MB;;;0;32000 'tbs_eoecwrk_usage_pct'=0.00%;90;98 'tbs_eoecwrk_usage'=0MB;28800;31360;0;32000 'tbs_eoecwrk_alloc'=5MB;;;0;32000 'tbs_eoeclrg_usage_pct'=0.00%;90;98 'tbs_eoeclrg_usage'=0MB;28800;31360;0;32000 'tbs_eoeclrg_alloc'=2MB;;;0;32000 'tbs_eoecapp_usage_pct'=0.01%;90;98 'tbs_eoecapp_usage'=4MB;28800;31360;0;32000 'tbs_eoecapp_alloc'=5MB;;;0;32000 'tbs_eodslrg_usage_pct'=0.00%;90;98 'tbs_eodslrg_usage'=0MB;28800;31360;0;32000 'tbs_eodslrg_alloc'=5MB;;;0;32000 'tbs_eodsapp_usage_pct'=0.01%;90;98 'tbs_eodsapp_usage'=3MB;28800;31360;0;32000 'tbs_eodsapp_alloc'=4MB;;;0;32000 'tbs_eoculrg_usage_pct'=0.25%;90;98 'tbs_eoculrg_usage'=80MB;28800;31360;0;32000 'tbs_eoculrg_alloc'=84MB;;;0;32000 'tbs_eocuapp_usage_pct'=0.00%;90;98 'tbs_eocuapp_usage'=0MB;28800;31360;0;32000 'tbs_eocuapp_alloc'=5MB;;;0;32000 'tbs_eocmwrk_usage_pct'=0.01%;90;98 'tbs_eocmwrk_usage'=2MB;28800;31360;0;32000 'tbs_eocmwrk_alloc'=3MB;;;0;32000 'tbs_eocmlrg_usage_pct'=0.00%;90;98 'tbs_eocmlrg_usage'=0MB;28800;31360;0;32000 'tbs_eocmlrg_alloc'=5MB;;;0;32000 'tbs_eocmapp_usage_pct'=0.08%;90;98 'tbs_eocmapp_usage'=27MB;28800;31360;0;32000 'tbs_eocmapp_alloc'=29MB;;;0;32000 'tbs_eocfapp_usage_pct'=0.04%;90;98 'tbs_eocfapp_usage'=12MB;28800;31360;0;32000 'tbs_eocfapp_alloc'=13MB;;;0;32000 'tbs_eobfapp_usage_pct'=0.00%;90;98 'tbs_eobfapp_usage'=0MB;28800;31360;0;32000 'tbs_eobfapp_alloc'=2MB;;;0;32000 'tbs_eoapp_usage_pct'=0.00%;90;98 'tbs_eoapp_usage'=0MB;28800;31360;0;32000 'tbs_eoapp_alloc'=5MB;;;0;32000 'tbs_e1ptwork_usage_pct'=0.01%;90;98 'tbs_e1ptwork_usage'=1MB;28800;31360;0;32000 'tbs_e1ptwork_alloc'=3MB;;;0;32000 'tbs_e1ptlarge_usage_pct'=0.00%;90;98 'tbs_e1ptlarge_usage'=0MB;28800;31360;0;32000 'tbs_e1ptlarge_alloc'=5MB;;;0;32000 'tbs_e1powork_usage_pct'=0.09%;90;98 'tbs_e1powork_usage'=29MB;28800;31360;0;32000 'tbs_e1powork_alloc'=141MB;;;0;32000 'tbs_e1polarge_usage_pct'=3.81%;90;98 'tbs_e1polarge_usage'=1220MB;28800;31360;0;32000 'tbs_e1polarge_alloc'=1224MB;;;0;32000 'tbs_e1pcwork_usage_pct'=0.23%;90;98 'tbs_e1pcwork_usage'=72MB;28800;31360;0;32000 'tbs_e1pcwork_alloc'=519MB;;;0;32000 'tbs_e1pclarge_usage_pct'=0.89%;90;98 'tbs_e1pclarge_usage'=285MB;28800;31360;0;32000 'tbs_e1pclarge_alloc'=619MB;;;0;32000 'tbs_e1omwork_usage_pct'=0.03%;90;98 'tbs_e1omwork_usage'=10MB;28800;31360;0;32000 'tbs_e1omwork_alloc'=56MB;;;0;32000 'tbs_e1omlarge_usage_pct'=0.07%;90;98 'tbs_e1omlarge_usage'=23MB;28800;31360;0;32000 'tbs_e1omlarge_alloc'=30MB;;;0;32000 'tbs_e1glwork_usage_pct'=0.06%;90;98 'tbs_e1glwork_usage'=20MB;28800;31360;0;32000 'tbs_e1glwork_alloc'=130MB;;;0;32000 'tbs_e1gllarge_usage_pct'=0.38%;90;98 'tbs_e1gllarge_usage'=121MB;28800;31360;0;32000 'tbs_e1gllarge_alloc'=134MB;;;0;32000 'tbs_e1fswork_usage_pct'=13.54%;90;98 'tbs_e1fswork_usage'=4333MB;28800;31360;0;32000 'tbs_e1fswork_alloc'=4445MB;;;0;32000 'tbs_e1fslarge_usage_pct'=5.88%;90;98 'tbs_e1fslarge_usage'=1881MB;28800;31360;0;32000 'tbs_e1fslarge_alloc'=1998MB;;;0;32000 'tbs_e1exwork_usage_pct'=0.04%;90;98 'tbs_e1exwork_usage'=12MB;28800;31360;0;32000 'tbs_e1exwork_alloc'=13MB;;;0;32000 'tbs_e1exlarge_usage_pct'=0.06%;90;98 'tbs_e1exlarge_usage'=19MB;28800;31360;0;32000 'tbs_e1exlarge_alloc'=30MB;;;0;32000 'tbs_e1cmwork_usage_pct'=0.01%;90;98 'tbs_e1cmwork_usage'=3MB;28800;31360;0;32000 'tbs_e1cmwork_alloc'=5MB;;;0;32000 'tbs_e1cmlarge_usage_pct'=0.10%;90;98 'tbs_e1cmlarge_usage'=31MB;28800;31360;0;32000 'tbs_e1cmlarge_alloc'=40MB;;;0;32000 'tbs_e1biwork_usage_pct'=0.06%;90;98 'tbs_e1biwork_usage'=18MB;28800;31360;0;32000 'tbs_e1biwork_alloc'=21MB;;;0;32000 'tbs_e1bilarge_usage_pct'=0.13%;90;98 'tbs_e1bilarge_usage'=40MB;28800;31360;0;32000 'tbs_e1bilarge_alloc'=126MB;;;0;32000 'tbs_e1arwork_usage_pct'=0.05%;90;98 'tbs_e1arwork_usage'=16MB;28800;31360;0;32000 'tbs_e1arwork_alloc'=30MB;;;0;32000 'tbs_e1arlarge_usage_pct'=0.29%;90;98 'tbs_e1arlarge_usage'=93MB;28800;31360;0;32000 'tbs_e1arlarge_alloc'=99MB;;;0;32000 'tbs_e1apwork_usage_pct'=0.36%;90;98 'tbs_e1apwork_usage'=114MB;28800;31360;0;32000 'tbs_e1apwork_alloc'=1981MB;;;0;32000 'tbs_e1aplarge_usage_pct'=0.32%;90;98 'tbs_e1aplarge_usage'=101MB;28800;31360;0;32000 'tbs_e1aplarge_alloc'=111MB;;;0;32000 'tbs_e1amwork_usage_pct'=0.03%;90;98 'tbs_e1amwork_usage'=8MB;28800;31360;0;32000 'tbs_e1amwork_alloc'=12MB;;;0;32000 'tbs_e1amlarge_usage_pct'=0.27%;90;98 'tbs_e1amlarge_usage'=85MB;28800;31360;0;32000 'tbs_e1amlarge_alloc'=90MB;;;0;32000 'tbs_dpwork_usage_pct'=0.00%;90;98 'tbs_dpwork_usage'=0MB;28800;31360;0;32000 'tbs_dpwork_alloc'=5MB;;;0;32000 'tbs_dplarge_usage_pct'=0.04%;90;98 'tbs_dplarge_usage'=12MB;28800;31360;0;32000 'tbs_dplarge_alloc'=14MB;;;0;32000 'tbs_dpapp_usage_pct'=0.09%;90;98 'tbs_dpapp_usage'=28MB;28800;31360;0;32000 'tbs_dpapp_alloc'=30MB;;;0;32000 'tbs_diapp_usage_pct'=0.00%;90;98 'tbs_diapp_usage'=0MB;28800;31360;0;32000 'tbs_diapp_alloc'=5MB;;;0;32000 'tbs_cularge_usage_pct'=0.01%;90;98 'tbs_cularge_usage'=2MB;28800;31360;0;32000 'tbs_cularge_alloc'=4MB;;;0;32000 'tbs_cularg3_usage_pct'=0.00%;90;98 'tbs_cularg3_usage'=0MB;28800;31360;0;32000 'tbs_cularg3_alloc'=2MB;;;0;32000 'tbs_cularg2_usage_pct'=0.00%;90;98 'tbs_cularg2_usage'=0MB;28800;31360;0;32000 'tbs_cularg2_alloc'=5MB;;;0;32000 'tbs_cularg1_usage_pct'=0.00%;90;98 'tbs_cularg1_usage'=0MB;28800;31360;0;32000 'tbs_cularg1_alloc'=5MB;;;0;32000 'tbs_cuaudit_usage_pct'=0.00%;90;98 'tbs_cuaudit_usage'=0MB;28800;31360;0;32000 'tbs_cuaudit_alloc'=5MB;;;0;32000 'tbs_cswork_usage_pct'=0.00%;90;98 'tbs_cswork_usage'=1MB;28800;31360;0;32000 'tbs_cswork_alloc'=3MB;;;0;32000 'tbs_cslarge_usage_pct'=0.01%;90;98 'tbs_cslarge_usage'=3MB;28800;31360;0;32000 'tbs_cslarge_alloc'=4MB;;;0;32000 'tbs_csapp_usage_pct'=0.03%;90;98 'tbs_csapp_usage'=8MB;28800;31360;0;32000 'tbs_csapp_alloc'=9MB;;;0;32000 'tbs_cplarge_usage_pct'=0.00%;90;98 'tbs_cplarge_usage'=0MB;28800;31360;0;32000 'tbs_cplarge_alloc'=5MB;;;0;32000 'tbs_cpapp_usage_pct'=0.02%;90;98 'tbs_cpapp_usage'=4MB;28800;31360;0;32000 'tbs_cpapp_alloc'=6MB;;;0;32000 'tbs_cfgapp_usage_pct'=0.00%;90;98 'tbs_cfgapp_usage'=0MB;28800;31360;0;32000 'tbs_cfgapp_alloc'=5MB;;;0;32000 'tbs_cawork_usage_pct'=0.15%;90;98 'tbs_cawork_usage'=49MB;28800;31360;0;32000 'tbs_cawork_alloc'=52MB;;;0;32000 'tbs_caapp_usage_pct'=0.20%;90;98 'tbs_caapp_usage'=65MB;28800;31360;0;32000 'tbs_caapp_alloc'=69MB;;;0;32000 'tbs_biwork_usage_pct'=0.37%;90;98 'tbs_biwork_usage'=119MB;28800;31360;0;32000 'tbs_biwork_alloc'=210MB;;;0;32000 'tbs_bilarge_usage_pct'=1.02%;90;98 'tbs_bilarge_usage'=324MB;28800;31360;0;32000 'tbs_bilarge_alloc'=345MB;;;0;32000 'tbs_biapp_usage_pct'=1.04%;90;98 'tbs_biapp_usage'=332MB;28800;31360;0;32000 'tbs_biapp_alloc'=343MB;;;0;32000 'tbs_bdlarge_usage_pct'=0.00%;90;98 'tbs_bdlarge_usage'=1MB;28800;31360;0;32000 'tbs_bdlarge_alloc'=2MB;;;0;32000 'tbs_bdapp_usage_pct'=0.04%;90;98 'tbs_bdapp_usage'=13MB;28800;31360;0;32000 'tbs_bdapp_alloc'=14MB;;;0;32000 'tbs_bcapp_usage_pct'=0.00%;90;98 'tbs_bcapp_usage'=0MB;28800;31360;0;32000 'tbs_bcapp_alloc'=5MB;;;0;32000 'tbs_auclrge_usage_pct'=0.00%;90;98 'tbs_auclrge_usage'=0MB;28800;31360;0;32000 'tbs_auclrge_alloc'=5MB;;;0;32000 'tbs_aucapp_usage_pct'=0.07%;90;98 'tbs_aucapp_usage'=23MB;28800;31360;0;32000 'tbs_aucapp_alloc'=25MB;;;0;32000 'tbs_arwork_usage_pct'=0.85%;90;98 'tbs_arwork_usage'=2");

    FILE *fdata = fopen (DB_NAME, "w");
    
    iniparser_dump_ini (ini, fdata);

    char **keys = iniparser_getseckeys (ini, "cache");
    char *key1 = keys[0];
    int nb = iniparser_getsecnkeys (ini, "cache");
    qsort (keys, (size_t) nb, sizeof (char *), compare);
    int i = 0;

    for (; i < nb; i++)
        fprintf (stdout, "key: %s\n", keys[i]);

    free (keys);

    fprintf (stdout, "key1: %s\n", key1);

    iniparser_freedict(ini);
    fclose (fdata);
    return 0;
}
