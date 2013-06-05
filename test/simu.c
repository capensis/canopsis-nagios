/*--------------------------------
# Copyright (c) 2011 "Capensis" [http://www.capensis.com]
#
# This file is part of Canopsis.
#
# Canopsis is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Canopsis is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Canopsis.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/time.h>

#include "nagios.h"

int event_broker_options = -1;

bool (*toggle_blackout)(void);
int (*service_check_data)(int,void *);
int (*host_check_data)(int,void *);

int write_to_all_logs(char * msg,unsigned long priority){
	printf("log: %s\n", msg);
	return 0;
}

int neb_register_callback(int callback_type, void *mod_handle, int priority, int (*callback_func)(int,void *)){
	if 			(callback_type == NEBCALLBACK_SERVICE_CHECK_DATA){
		service_check_data = callback_func;
		
	}else if	(callback_type == NEBCALLBACK_HOST_CHECK_DATA){
		host_check_data = callback_func;
	}
	return 0;
}

int neb_deregister_callback(int callback_type, int (*callback_func)(int,void *)){
	return 0;
}

nebstruct_service_check_data build_service_check(char * host_name, char * description, char * plugin_output){
	nebstruct_service_check_data ds;
	
	struct timeval tv;
	gettimeofday(&tv,NULL);

	host host_object;
	host_object.address="127.0.0.1";
	
	service service_object;
	service_object.host_ptr = &host_object;

	ds.type=NEBTYPE_SERVICECHECK_PROCESSED;
	ds.flags=0;
	ds.attr=0;
	ds.timestamp=tv;

	ds.host_name=host_name;
	ds.service_description=description;
	
	ds.object_ptr=&service_object;
	
	ds.check_type=0;
	ds.current_attempt=1;
	ds.max_attempts=5;
	ds.state=0;
	ds.state_type=1;
	ds.timeout=0;
	ds.command_name="check_debug";
	ds.command_args="";
	ds.command_line="";
	ds.start_time=tv;
	ds.end_time=tv;
	ds.early_timeout=0;
	ds.execution_time=0.23;
	ds.latency=0.55;
	ds.return_code=0;
	ds.output=plugin_output;
	ds.long_output="";
	ds.perf_data="";
	//ds.perf_data="'tbs_wslarge_usage_pct'=0.00%;90;98 'tbs_wslarge_usage'=0MB;28800;31360;0;32000 'tbs_wslarge_alloc'=5MB;;;0;32000 'tbs_wsapp_usage_pct'=0.01%;90;98 'tbs_wsapp_usage'=2MB;28800;31360;0;32000 'tbs_wsapp_alloc'=3MB;;;0;32000 'tbs_wmwork_usage_pct'=0.05%;90;98 'tbs_wmwork_usage'=17MB;28800;31360;0;32000 'tbs_wmwork_alloc'=18MB;;;0;32000 'tbs_wmlarge_usage_pct'=0.06%;90;98 'tbs_wmlarge_usage'=18MB;28800;31360;0;32000 'tbs_wmlarge_alloc'=19MB;;;0;32000 'tbs_wmimage_usage_pct'=0.00%;90;98 'tbs_wmimage_usage'=0MB;28800;31360;0;32000 'tbs_wmimage_alloc'=5MB;;;0;32000 'tbs_wmapp_usage_pct'=0.08%;90;98 'tbs_wmapp_usage'=26MB;28800;31360;0;32000 'tbs_wmapp_alloc'=28MB;;;0;32000 'tbs_trwork_usage_pct'=0.10%;90;98 'tbs_trwork_usage'=32MB;28800;31360;0;32000 'tbs_trwork_alloc'=35MB;;;0;32000 'tbs_trlarge_usage_pct'=0.21%;90;98 'tbs_trlarge_usage'=66MB;28800;31360;0;32000 'tbs_trlarge_alloc'=67MB;;;0;32000 'tbs_trimage_usage_pct'=1.45%;90;98 'tbs_trimage_usage'=465MB;28800;31360;0;32000 'tbs_trimage_alloc'=489MB;;;0;32000 'tbs_trarch_usage_pct'=0.00%;90;98 'tbs_trarch_usage'=0MB;28800;31360;0;32000 'tbs_trarch_alloc'=5MB;;;0;32000 'tbs_trapp_usage_pct'=1.55%;90;98 'tbs_trapp_usage'=496MB;28800;31360;0;32000 'tbs_trapp_alloc'=518MB;;;0;32000 'tbs_tpmapp_usage_pct'=0.00%;90;98 'tbs_tpmapp_usage'=0MB;28800;31360;0;32000 'tbs_tpmapp_alloc'=5MB;;;0;32000 'tbs_tools_usage_pct'=6.93%;90;98 'tbs_tools_usage'=2219MB;28800;31360;0;32000 'tbs_tools_alloc'=2300MB;;;0;32000 'tbs_tlwork_usage_pct'=0.00%;90;98 'tbs_tlwork_usage'=0MB;28800;31360;0;32000 'tbs_tlwork_alloc'=5MB;;;0;32000 'tbs_tlapp_usage_pct'=0.00%;90;98 'tbs_tlapp_usage'=0MB;28800;31360;0;32000 'tbs_tlapp_alloc'=2MB;;;0;32000 'tbs_temp02_usage_pct'=0.04%;90;98 'tbs_temp02_usage'=6MB;13500;14700;0;15000 'tbs_temp02_alloc'=15000MB;;;0;15000 'tbs_temp01_usage_pct'=0.00%;90;98 'tbs_temp01_usage'=0MB;13500;14700;0;15000 'tbs_temp01_alloc'=15000MB;;;0;15000 'tbs_tdwork_usage_pct'=0.01%;90;98 'tbs_tdwork_usage'=4MB;28800;31360;0;32000 'tbs_tdwork_alloc'=6MB;;;0;32000 'tbs_tdlarge_usage_pct'=0.01%;90;98 'tbs_tdlarge_usage'=4MB;28800;31360;0;32000 'tbs_tdlarge_alloc'=6MB;;;0;32000 'tbs_tdapp_usage_pct'=0.02%;90;98 'tbs_tdapp_usage'=4MB;28800;31360;0;32000 'tbs_tdapp_alloc'=6MB;;;0;32000 'tbs_system_usage_pct'=80.47%;90;98 'tbs_system_usage'=2011MB;2250;2450;0;2500 'tbs_system_alloc'=2500MB;;;0;2500 'tbs_sysaux_usage_pct'=82.93%;90;98 'tbs_sysaux_usage'=2902MB;3150;3430;0;3500 'tbs_sysaux_alloc'=3500MB;;;0;3500 'tbs_splarge_usage_pct'=0.01%;90;98 'tbs_splarge_usage'=4MB;28800;31360;0;32000 'tbs_splarge_alloc'=6MB;;;0;32000 'tbs_spapp_usage_pct'=0.23%;90;98 'tbs_spapp_usage'=74MB;28800;31360;0;32000 'tbs_spapp_alloc'=78MB;;;0;32000 'tbs_saclrge_usage_pct'=0.00%;90;98 'tbs_saclrge_usage'=0MB;28800;31360;0;32000 'tbs_saclrge_alloc'=5MB;;;0;32000 'tbs_sacapp_usage_pct'=0.02%;90;98 'tbs_sacapp_usage'=5MB;28800;31360;0;32000 'tbs_sacapp_alloc'=6MB;;;0;32000 'tbs_rswork_usage_pct'=0.00%;90;98 'tbs_rswork_usage'=0MB;28800;31360;0;32000 'tbs_rswork_alloc'=5MB;;;0;32000 'tbs_rslarge_usage_pct'=0.02%;90;98 'tbs_rslarge_usage'=7MB;28800;31360;0;32000 'tbs_rslarge_alloc'=9MB;;;0;32000 'tbs_rsfapp_usage_pct'=0.00%;90;98 'tbs_rsfapp_usage'=0MB;28800;31360;0;32000 'tbs_rsfapp_alloc'=5MB;;;0;32000 'tbs_rsapp_usage_pct'=0.09%;90;98 'tbs_rsapp_usage'=29MB;28800;31360;0;32000 'tbs_rsapp_alloc'=32MB;;;0;32000 'tbs_roapp_usage_pct'=0.00%;90;98 'tbs_roapp_usage'=0MB;28800;31360;0;32000 'tbs_roapp_alloc'=5MB;;;0;32000 'tbs_rework_usage_pct'=0.03%;90;98 'tbs_rework_usage'=9MB;28800;31360;0;32000 'tbs_rework_alloc'=10MB;;;0;32000 'tbs_relarge_usage_pct'=0.03%;90;98 'tbs_relarge_usage'=9MB;28800;31360;0;32000 'tbs_relarge_alloc'=11MB;;;0;32000 'tbs_reimage_usage_pct'=0.00%;90;98 'tbs_reimage_usage'=0MB;28800;31360;0;32000 'tbs_reimage_alloc'=5MB;;;0;32000 'tbs_reapp_usage_pct'=0.03%;90;98 'tbs_reapp_usage'=9MB;28800;31360;0;32000 'tbs_reapp_alloc'=11MB;;;0;32000 'tbs_rbtemp_usage_pct'=0.00%;90;98 'tbs_rbtemp_usage'=0MB;28800;31360;0;32000 'tbs_rbtemp_alloc'=5MB;;;0;32000 'tbs_rbapp_usage_pct'=0.00%;90;98 'tbs_rbapp_usage'=0MB;28800;31360;0;32000 'tbs_rbapp_alloc'=5MB;;;0;32000 'tbs_pyapp_usage_pct'=0.00%;90;98 'tbs_pyapp_usage'=0MB;28800;31360;0;32000 'tbs_pyapp_alloc'=2MB;;;0;32000 'tbs_pvlarge_usage_pct'=0.00%;90;98 'tbs_pvlarge_usage'=0MB;28800;31360;0;32000 'tbs_pvlarge_alloc'=5MB;;;0;32000 'tbs_pvapp_usage_pct'=0.20%;90;98 'tbs_pvapp_usage'=63MB;28800;31360;0;32000 'tbs_pvapp_alloc'=67MB;;;0;32000 'tbs_ptwork_usage_pct'=0.01%;90;98 'tbs_ptwork_usage'=4MB;28800;31360;0;32000 'tbs_ptwork_alloc'=5MB;;;0;32000 'tbs_pttree_usage_pct'=0.02%;90;98 'tbs_pttree_usage'=7MB;28800;31360;0;32000 'tbs_pttree_alloc'=12MB;;;0;32000 'tbs_pttlrg_usage_pct'=4.23%;90;98 'tbs_pttlrg_usage'=1352MB;28800;31360;0;32000 'tbs_pttlrg_alloc'=3618MB;;;0;32000 'tbs_pttbl_usage_pct'=2.33%;90;98 'tbs_pttbl_usage'=746MB;28800;31360;0;32000 'tbs_pttbl_alloc'=1369MB;;;0;32000 'tbs_ptrpts_usage_pct'=0.47%;90;98 'tbs_ptrpts_usage'=150MB;28800;31360;0;32000 'tbs_ptrpts_alloc'=620MB;;;0;32000 'tbs_ptprjwk_usage_pct'=0.00%;90;98 'tbs_ptprjwk_usage'=0MB;28800;31360;0;32000 'tbs_ptprjwk_alloc'=5MB;;;0;32000 'tbs_ptprc_usage_pct'=1.84%;90;98 'tbs_ptprc_usage'=588MB;28800;31360;0;32000 'tbs_ptprc_alloc'=878MB;;;0;32000 'tbs_ptlock_usage_pct'=0.00%;90;98 'tbs_ptlock_usage'=0MB;28800;31360;0;32000 'tbs_ptlock_alloc'=5MB;;;0;32000 'tbs_ptcmstar_usage_pct'=0.00%;90;98 'tbs_ptcmstar_usage'=0MB;28800;31360;0;32000 'tbs_ptcmstar_alloc'=5MB;;;0;32000 'tbs_ptaudit_usage_pct'=0.02%;90;98 'tbs_ptaudit_usage'=5MB;28800;31360;0;32000 'tbs_ptaudit_alloc'=6MB;;;0;32000 'tbs_ptappe_usage_pct'=0.03%;90;98 'tbs_ptappe_usage'=8MB;28800;31360;0;32000 'tbs_ptappe_alloc'=10MB;;;0;32000 'tbs_ptapp_usage_pct'=0.09%;90;98 'tbs_ptapp_usage'=30MB;28800;31360;0;32000 'tbs_ptapp_alloc'=57MB;;;0;32000 'tbs_ptamsg_usage_pct'=0.04%;90;98 'tbs_ptamsg_usage'=13MB;28800;31360;0;32000 'tbs_ptamsg_alloc'=281MB;;;0;32000 'tbs_pswork_usage_pct'=0.00%;90;98 'tbs_pswork_usage'=0MB;28800;31360;0;32000 'tbs_pswork_alloc'=5MB;;;0;32000 'tbs_psundots_usage_pct'=67.24%;90;98 'tbs_psundots_usage'=20172MB;27000;29400;0;30000 'tbs_psundots_alloc'=30000MB;;;0;30000 'tbs_psindex_usage_pct'=23.88%;90;98 'tbs_psindex_usage'=38213MB;144000;156800;0;160000 'tbs_psindex_alloc'=47000MB;;;0;160000 'tbs_psimgr_usage_pct'=0.01%;90;98 'tbs_psimgr_usage'=1MB;28800;31360;0;32000 'tbs_psimgr_alloc'=3MB;;;0;32000 'tbs_psimage_usage_pct'=5.43%;90;98 'tbs_psimage_usage'=1739MB;28800;31360;0;32000 'tbs_psimage_alloc'=23660MB;;;0;32000 'tbs_psdefault_usage_pct'=10.82%;90;98 'tbs_psdefault_usage'=3463MB;28800;31360;0;32000 'tbs_psdefault_alloc'=5440MB;;;0;32000 'tbs_powork_usage_pct'=0.64%;90;98 'tbs_powork_usage'=203MB;28800;31360;0;32000 'tbs_powork_alloc'=215MB;;;0;32000 'tbs_polarge_usage_pct'=4.29%;90;98 'tbs_polarge_usage'=1373MB;28800;31360;0;32000 'tbs_polarge_alloc'=1407MB;;;0;32000 'tbs_poapp_usage_pct'=3.14%;90;98 'tbs_poapp_usage'=1004MB;28800;31360;0;32000 'tbs_poapp_alloc'=1413MB;;;0;32000 'tbs_plwork_usage_pct'=0.01%;90;98 'tbs_plwork_usage'=4MB;28800;31360;0;32000 'tbs_plwork_alloc'=6MB;;;0;32000 'tbs_pllarge_usage_pct'=0.06%;90;98 'tbs_pllarge_usage'=20MB;28800;31360;0;32000 'tbs_pllarge_alloc'=21MB;;;0;32000 'tbs_plapp_usage_pct'=0.09%;90;98 'tbs_plapp_usage'=27MB;28800;31360;0;32000 'tbs_plapp_alloc'=30MB;;;0;32000 'tbs_pipswork_usage_pct'=0.00%;90;98 'tbs_pipswork_usage'=0MB;28800;31360;0;32000 'tbs_pipswork_alloc'=5MB;;;0;32000 'tbs_pipslrg_usage_pct'=0.00%;90;98 'tbs_pipslrg_usage'=0MB;28800;31360;0;32000 'tbs_pipslrg_alloc'=5MB;;;0;32000 'tbs_pipsapp_usage_pct'=0.00%;90;98 'tbs_pipsapp_usage'=0MB;28800;31360;0;32000 'tbs_pipsapp_alloc'=5MB;;;0;32000 'tbs_pgmwork_usage_pct'=0.11%;90;98 'tbs_pgmwork_usage'=36MB;28800;31360;0;32000 'tbs_pgmwork_alloc'=38MB;;;0;32000 'tbs_pgmlarge_usage_pct'=0.02%;90;98 'tbs_pgmlarge_usage'=6MB;28800;31360;0;32000 'tbs_pgmlarge_alloc'=7MB;;;0;32000 'tbs_pgmapp_usage_pct'=0.07%;90;98 'tbs_pgmapp_usage'=21MB;28800;31360;0;32000 'tbs_pgmapp_alloc'=23MB;;;0;32000 'tbs_pcwork_usage_pct'=0.00%;90;98 'tbs_pcwork_usage'=0MB;28800;31360;0;32000 'tbs_pcwork_alloc'=5MB;;;0;32000 'tbs_pclarge_usage_pct'=5.14%;90;98 'tbs_pclarge_usage'=1644MB;28800;31360;0;32000 'tbs_pclarge_alloc'=1695MB;;;0;32000 'tbs_pcarch_usage_pct'=0.00%;90;98 'tbs_pcarch_usage'=0MB;28800;31360;0;32000 'tbs_pcarch_alloc'=5MB;;;0;32000 'tbs_pcapp_usage_pct'=1.54%;90;98 'tbs_pcapp_usage'=493MB;28800;31360;0;32000 'tbs_pcapp_alloc'=637MB;;;0;32000 'tbs_pbwork_usage_pct'=0.00%;90;98 'tbs_pbwork_usage'=0MB;28800;31360;0;32000 'tbs_pbwork_alloc'=5MB;;;0;32000 'tbs_pblarge_usage_pct'=0.00%;90;98 'tbs_pblarge_usage'=0MB;28800;31360;0;32000 'tbs_pblarge_alloc'=5MB;;;0;32000 'tbs_pbimage_usage_pct'=0.00%;90;98 'tbs_pbimage_usage'=0MB;28800;31360;0;32000 'tbs_pbimage_alloc'=5MB;;;0;32000 'tbs_pbapp_usage_pct'=0.00%;90;98 'tbs_pbapp_usage'=0MB;28800;31360;0;32000 'tbs_pbapp_alloc'=5MB;;;0;32000 'tbs_omwork_usage_pct'=1.06%;90;98 'tbs_omwork_usage'=338MB;28800;31360;0;32000 'tbs_omwork_alloc'=355MB;;;0;32000 'tbs_omlarge_usage_pct'=0.10%;90;98 'tbs_omlarge_usage'=31MB;28800;31360;0;32000 'tbs_omlarge_alloc'=38MB;;;0;32000 'tbs_omapp_usage_pct'=0.13%;90;98 'tbs_omapp_usage'=40MB;28800;31360;0;32000 'tbs_omapp_alloc'=116MB;;;0;32000 'tbs_oeapp_usage_pct'=0.00%;90;98 'tbs_oeapp_usage'=0MB;28800;31360;0;32000 'tbs_oeapp_alloc'=5MB;;;0;32000 'tbs_mpwork_usage_pct'=0.00%;90;98 'tbs_mpwork_usage'=0MB;28800;31360;0;32000 'tbs_mpwork_alloc'=5MB;;;0;32000 'tbs_mplarge_usage_pct'=0.00%;90;98 'tbs_mplarge_usage'=0MB;28800;31360;0;32000 'tbs_mplarge_alloc'=5MB;;;0;32000 'tbs_mpapp_usage_pct'=0.03%;90;98 'tbs_mpapp_usage'=9MB;28800;31360;0;32000 'tbs_mpapp_alloc'=10MB;;;0;32000 'tbs_mglarge_usage_pct'=0.02%;90;98 'tbs_mglarge_usage'=4MB;28800;31360;0;32000 'tbs_mglarge_alloc'=6MB;;;0;32000 'tbs_mgapp_usage_pct'=0.30%;90;98 'tbs_mgapp_usage'=95MB;28800;31360;0;32000 'tbs_mgapp_alloc'=100MB;;;0;32000 'tbs_lclarge_usage_pct'=19.57%;90;98 'tbs_lclarge_usage'=6261MB;28800;31360;0;32000 'tbs_lclarge_alloc'=6644MB;;;0;32000 'tbs_lcapp_usage_pct'=2.13%;90;98 'tbs_lcapp_usage'=681MB;28800;31360;0;32000 'tbs_lcapp_alloc'=792MB;;;0;32000 'tbs_inwork_usage_pct'=0.73%;90;98 'tbs_inwork_usage'=234MB;28800;31360;0;32000 'tbs_inwork_alloc'=246MB;;;0;32000 'tbs_inlarge_usage_pct'=12.95%;90;98 'tbs_inlarge_usage'=4144MB;28800;31360;0;32000 'tbs_inlarge_alloc'=4352MB;;;0;32000 'tbs_inarch_usage_pct'=0.00%;90;98 'tbs_inarch_usage'=1MB;28800;31360;0;32000 'tbs_inarch_alloc'=2MB;;;0;32000 'tbs_inapp_usage_pct'=2.84%;90;98 'tbs_inapp_usage'=908MB;28800;31360;0;32000 'tbs_inapp_alloc'=954MB;;;0;32000 'tbs_htapp_usage_pct'=0.00%;90;98 'tbs_htapp_usage'=0MB;28800;31360;0;32000 'tbs_htapp_alloc'=2MB;;;0;32000 'tbs_hrlarge_usage_pct'=0.07%;90;98 'tbs_hrlarge_usage'=23MB;28800;31360;0;32000 'tbs_hrlarge_alloc'=26MB;;;0;32000 'tbs_hrapp_usage_pct'=0.07%;90;98 'tbs_hrapp_usage'=23MB;28800;31360;0;32000 'tbs_hrapp_alloc'=32MB;;;0;32000 'tbs_gmapp_usage_pct'=0.15%;90;98 'tbs_gmapp_usage'=47MB;28800;31360;0;32000 'tbs_gmapp_alloc'=50MB;;;0;32000 'tbs_glwork_usage_pct'=0.28%;90;98 'tbs_glwork_usage'=90MB;28800;31360;0;32000 'tbs_glwork_alloc'=105MB;;;0;32000 'tbs_gllarge_usage_pct'=3.75%;90;98 'tbs_gllarge_usage'=1198MB;28800;31360;0;32000 'tbs_gllarge_alloc'=1201MB;;;0;32000 'tbs_glarch_usage_pct'=0.02%;90;98 'tbs_glarch_usage'=5MB;28800;31360;0;32000 'tbs_glarch_alloc'=7MB;;;0;32000 'tbs_glapp_usage_pct'=1.37%;90;98 'tbs_glapp_usage'=438MB;28800;31360;0;32000 'tbs_glapp_alloc'=612MB;;;0;32000 'tbs_fswork_usage_pct'=0.23%;90;98 'tbs_fswork_usage'=72MB;28800;31360;0;32000 'tbs_fswork_alloc'=76MB;;;0;32000 'tbs_fslarge_usage_pct'=2.83%;90;98 'tbs_fslarge_usage'=904MB;28800;31360;0;32000 'tbs_fslarge_alloc'=924MB;;;0;32000 'tbs_fsapp1_usage_pct'=0.00%;90;98 'tbs_fsapp1_usage'=1MB;28800;31360;0;32000 'tbs_fsapp1_alloc'=2MB;;;0;32000 'tbs_fsapp_usage_pct'=9.64%;90;98 'tbs_fsapp_usage'=3086MB;28800;31360;0;32000 'tbs_fsapp_alloc'=3163MB;;;0;32000 'tbs_fowork_usage_pct'=0.00%;90;98 'tbs_fowork_usage'=0MB;28800;31360;0;32000 'tbs_fowork_alloc'=5MB;;;0;32000 'tbs_folarge_usage_pct'=0.02%;90;98 'tbs_folarge_usage'=5MB;28800;31360;0;32000 'tbs_folarge_alloc'=6MB;;;0;32000 'tbs_foimage_usage_pct'=0.00%;90;98 'tbs_foimage_usage'=0MB;28800;31360;0;32000 'tbs_foimage_alloc'=5MB;;;0;32000 'tbs_foapp_usage_pct'=0.03%;90;98 'tbs_foapp_usage'=8MB;28800;31360;0;32000 'tbs_foapp_alloc'=10MB;;;0;32000 'tbs_filarge_usage_pct'=0.00%;90;98 'tbs_filarge_usage'=0MB;28800;31360;0;32000 'tbs_filarge_alloc'=5MB;;;0;32000 'tbs_fiapp_usage_pct'=0.00%;90;98 'tbs_fiapp_usage'=0MB;28800;31360;0;32000 'tbs_fiapp_alloc'=5MB;;;0;32000 'tbs_fgwork_usage_pct'=0.00%;90;98 'tbs_fgwork_usage'=0MB;28800;31360;0;32000 'tbs_fgwork_alloc'=5MB;;;0;32000 'tbs_fgtemp_usage_pct'=0.00%;90;98 'tbs_fgtemp_usage'=0MB;28800;31360;0;32000 'tbs_fgtemp_alloc'=5MB;;;0;32000 'tbs_fglarge_usage_pct'=0.02%;90;98 'tbs_fglarge_usage'=4MB;28800;31360;0;32000 'tbs_fglarge_alloc'=6MB;;;0;32000 'tbs_fgimage_usage_pct'=0.00%;90;98 'tbs_fgimage_usage'=0MB;28800;31360;0;32000 'tbs_fgimage_alloc'=5MB;;;0;32000 'tbs_fgarch_usage_pct'=0.00%;90;98 'tbs_fgarch_usage'=0MB;28800;31360;0;32000 'tbs_fgarch_alloc'=5MB;;;0;32000 'tbs_fgapp_usage_pct'=0.01%;90;98 'tbs_fgapp_usage'=2MB;28800;31360;0;32000 'tbs_fgapp_alloc'=4MB;;;0;32000 'tbs_fawork_usage_pct'=0.00%;90;98 'tbs_fawork_usage'=0MB;28800;31360;0;32000 'tbs_fawork_alloc'=5MB;;;0;32000 'tbs_falarge_usage_pct'=0.00%;90;98 'tbs_falarge_usage'=0MB;28800;31360;0;32000 'tbs_falarge_alloc'=5MB;;;0;32000 'tbs_faapp_usage_pct'=0.04%;90;98 'tbs_faapp_usage'=11MB;28800;31360;0;32000 'tbs_faapp_alloc'=13MB;;;0;32000 'tbs_exwork_usage_pct'=0.35%;90;98 'tbs_exwork_usage'=111MB;28800;31360;0;32000 'tbs_exwork_alloc'=181MB;;;0;32000 'tbs_exlarge_usage_pct'=1.19%;90;98 'tbs_exlarge_usage'=379MB;28800;31360;0;32000 'tbs_exlarge_alloc'=398MB;;;0;32000 'tbs_exarch_usage_pct'=0.01%;90;98 'tbs_exarch_usage'=3MB;28800;31360;0;32000 'tbs_exarch_alloc'=4MB;;;0;32000 'tbs_exapp_usage_pct'=0.04%;90;98 'tbs_exapp_usage'=13MB;28800;31360;0;32000 'tbs_exapp_alloc'=15MB;;;0;32000 'tbs_eotplrg_usage_pct'=0.00%;90;98 'tbs_eotplrg_usage'=1MB;28800;31360;0;32000 'tbs_eotplrg_alloc'=2MB;;;0;32000 'tbs_eotpapp_usage_pct'=0.01%;90;98 'tbs_eotpapp_usage'=2MB;28800;31360;0;32000 'tbs_eotpapp_alloc'=4MB;;;0;32000 'tbs_eopplrg_usage_pct'=0.00%;90;98 'tbs_eopplrg_usage'=0MB;28800;31360;0;32000 'tbs_eopplrg_alloc'=5MB;;;0;32000 'tbs_eoppapp_usage_pct'=0.01%;90;98 'tbs_eoppapp_usage'=3MB;28800;31360;0;32000 'tbs_eoppapp_alloc'=5MB;;;0;32000 'tbs_eoltapp_usage_pct'=0.01%;90;98 'tbs_eoltapp_usage'=2MB;28800;31360;0;32000 'tbs_eoltapp_alloc'=4MB;;;0;32000 'tbs_eolarge_usage_pct'=0.00%;90;98 'tbs_eolarge_usage'=0MB;28800;31360;0;32000 'tbs_eolarge_alloc'=5MB;;;0;32000 'tbs_eoiuwrk_usage_pct'=0.00%;90;98 'tbs_eoiuwrk_usage'=0MB;28800;31360;0;32000 'tbs_eoiuwrk_alloc'=5MB;;;0;32000 'tbs_eoiulrg_usage_pct'=0.00%;90;98 'tbs_eoiulrg_usage'=0MB;28800;31360;0;32000 'tbs_eoiulrg_alloc'=5MB;;;0;32000 'tbs_eoiuapp_usage_pct'=1.80%;90;98 'tbs_eoiuapp_usage'=576MB;28800;31360;0;32000 'tbs_eoiuapp_alloc'=605MB;;;0;32000 'tbs_eoewwrk_usage_pct'=0.00%;90;98 'tbs_eoewwrk_usage'=0MB;28800;31360;0;32000 'tbs_eoewwrk_alloc'=5MB;;;0;32000 'tbs_eoewlrg_usage_pct'=0.00%;90;98 'tbs_eoewlrg_usage'=0MB;28800;31360;0;32000 'tbs_eoewlrg_alloc'=5MB;;;0;32000 'tbs_eoewapp_usage_pct'=0.02%;90;98 'tbs_eoewapp_usage'=5MB;28800;31360;0;32000 'tbs_eoewapp_alloc'=7MB;;;0;32000 'tbs_eoepwrk_usage_pct'=0.04%;90;98 'tbs_eoepwrk_usage'=12MB;28800;31360;0;32000 'tbs_eoepwrk_alloc'=13MB;;;0;32000 'tbs_eoeplrg_usage_pct'=0.00%;90;98 'tbs_eoeplrg_usage'=0MB;28800;31360;0;32000 'tbs_eoeplrg_alloc'=5MB;;;0;32000 'tbs_eoepapp_usage_pct'=0.02%;90;98 'tbs_eoepapp_usage'=7MB;28800;31360;0;32000 'tbs_eoepapp_alloc'=8MB;;;0;32000 'tbs_eoeilrg_usage_pct'=0.00%;90;98 'tbs_eoeilrg_usage'=0MB;28800;31360;0;32000 'tbs_eoeilrg_alloc'=5MB;;;0;32000 'tbs_eoeiapp_usage_pct'=0.04%;90;98 'tbs_eoeiapp_usage'=13MB;28800;31360;0;32000 'tbs_eoeiapp_alloc'=15MB;;;0;32000 'tbs_eoecwrk_usage_pct'=0.00%;90;98 'tbs_eoecwrk_usage'=0MB;28800;31360;0;32000 'tbs_eoecwrk_alloc'=5MB;;;0;32000 'tbs_eoeclrg_usage_pct'=0.00%;90;98 'tbs_eoeclrg_usage'=0MB;28800;31360;0;32000 'tbs_eoeclrg_alloc'=2MB;;;0;32000 'tbs_eoecapp_usage_pct'=0.01%;90;98 'tbs_eoecapp_usage'=4MB;28800;31360;0;32000 'tbs_eoecapp_alloc'=5MB;;;0;32000 'tbs_eodslrg_usage_pct'=0.00%;90;98 'tbs_eodslrg_usage'=0MB;28800;31360;0;32000 'tbs_eodslrg_alloc'=5MB;;;0;32000 'tbs_eodsapp_usage_pct'=0.01%;90;98 'tbs_eodsapp_usage'=3MB;28800;31360;0;32000 'tbs_eodsapp_alloc'=4MB;;;0;32000 'tbs_eoculrg_usage_pct'=0.25%;90;98 'tbs_eoculrg_usage'=80MB;28800;31360;0;32000 'tbs_eoculrg_alloc'=84MB;;;0;32000 'tbs_eocuapp_usage_pct'=0.00%;90;98 'tbs_eocuapp_usage'=0MB;28800;31360;0;32000 'tbs_eocuapp_alloc'=5MB;;;0;32000 'tbs_eocmwrk_usage_pct'=0.01%;90;98 'tbs_eocmwrk_usage'=2MB;28800;31360;0;32000 'tbs_eocmwrk_alloc'=3MB;;;0;32000 'tbs_eocmlrg_usage_pct'=0.00%;90;98 'tbs_eocmlrg_usage'=0MB;28800;31360;0;32000 'tbs_eocmlrg_alloc'=5MB;;;0;32000 'tbs_eocmapp_usage_pct'=0.08%;90;98 'tbs_eocmapp_usage'=27MB;28800;31360;0;32000 'tbs_eocmapp_alloc'=29MB;;;0;32000 'tbs_eocfapp_usage_pct'=0.04%;90;98 'tbs_eocfapp_usage'=12MB;28800;31360;0;32000 'tbs_eocfapp_alloc'=13MB;;;0;32000 'tbs_eobfapp_usage_pct'=0.00%;90;98 'tbs_eobfapp_usage'=0MB;28800;31360;0;32000 'tbs_eobfapp_alloc'=2MB;;;0;32000 'tbs_eoapp_usage_pct'=0.00%;90;98 'tbs_eoapp_usage'=0MB;28800;31360;0;32000 'tbs_eoapp_alloc'=5MB;;;0;32000 'tbs_e1ptwork_usage_pct'=0.01%;90;98 'tbs_e1ptwork_usage'=1MB;28800;31360;0;32000 'tbs_e1ptwork_alloc'=3MB;;;0;32000 'tbs_e1ptlarge_usage_pct'=0.00%;90;98 'tbs_e1ptlarge_usage'=0MB;28800;31360;0;32000 'tbs_e1ptlarge_alloc'=5MB;;;0;32000 'tbs_e1powork_usage_pct'=0.09%;90;98 'tbs_e1powork_usage'=29MB;28800;31360;0;32000 'tbs_e1powork_alloc'=141MB;;;0;32000 'tbs_e1polarge_usage_pct'=3.81%;90;98 'tbs_e1polarge_usage'=1220MB;28800;31360;0;32000 'tbs_e1polarge_alloc'=1224MB;;;0;32000 'tbs_e1pcwork_usage_pct'=0.23%;90;98 'tbs_e1pcwork_usage'=72MB;28800;31360;0;32000 'tbs_e1pcwork_alloc'=519MB;;;0;32000 'tbs_e1pclarge_usage_pct'=0.89%;90;98 'tbs_e1pclarge_usage'=285MB;28800;31360;0;32000 'tbs_e1pclarge_alloc'=619MB;;;0;32000 'tbs_e1omwork_usage_pct'=0.03%;90;98 'tbs_e1omwork_usage'=10MB;28800;31360;0;32000 'tbs_e1omwork_alloc'=56MB;;;0;32000 'tbs_e1omlarge_usage_pct'=0.07%;90;98 'tbs_e1omlarge_usage'=23MB;28800;31360;0;32000 'tbs_e1omlarge_alloc'=30MB;;;0;32000 'tbs_e1glwork_usage_pct'=0.06%;90;98 'tbs_e1glwork_usage'=20MB;28800;31360;0;32000 'tbs_e1glwork_alloc'=130MB;;;0;32000 'tbs_e1gllarge_usage_pct'=0.38%;90;98 'tbs_e1gllarge_usage'=121MB;28800;31360;0;32000 'tbs_e1gllarge_alloc'=134MB;;;0;32000 'tbs_e1fswork_usage_pct'=13.54%;90;98 'tbs_e1fswork_usage'=4333MB;28800;31360;0;32000 'tbs_e1fswork_alloc'=4445MB;;;0;32000 'tbs_e1fslarge_usage_pct'=5.88%;90;98 'tbs_e1fslarge_usage'=1881MB;28800;31360;0;32000 'tbs_e1fslarge_alloc'=1998MB;;;0;32000 'tbs_e1exwork_usage_pct'=0.04%;90;98 'tbs_e1exwork_usage'=12MB;28800;31360;0;32000 'tbs_e1exwork_alloc'=13MB;;;0;32000 'tbs_e1exlarge_usage_pct'=0.06%;90;98 'tbs_e1exlarge_usage'=19MB;28800;31360;0;32000 'tbs_e1exlarge_alloc'=30MB;;;0;32000 'tbs_e1cmwork_usage_pct'=0.01%;90;98 'tbs_e1cmwork_usage'=3MB;28800;31360;0;32000 'tbs_e1cmwork_alloc'=5MB;;;0;32000 'tbs_e1cmlarge_usage_pct'=0.10%;90;98 'tbs_e1cmlarge_usage'=31MB;28800;31360;0;32000 'tbs_e1cmlarge_alloc'=40MB;;;0;32000 'tbs_e1biwork_usage_pct'=0.06%;90;98 'tbs_e1biwork_usage'=18MB;28800;31360;0;32000 'tbs_e1biwork_alloc'=21MB;;;0;32000 'tbs_e1bilarge_usage_pct'=0.13%;90;98 'tbs_e1bilarge_usage'=40MB;28800;31360;0;32000 'tbs_e1bilarge_alloc'=126MB;;;0;32000 'tbs_e1arwork_usage_pct'=0.05%;90;98 'tbs_e1arwork_usage'=16MB;28800;31360;0;32000 'tbs_e1arwork_alloc'=30MB;;;0;32000 'tbs_e1arlarge_usage_pct'=0.29%;90;98 'tbs_e1arlarge_usage'=93MB;28800;31360;0;32000 'tbs_e1arlarge_alloc'=99MB;;;0;32000 'tbs_e1apwork_usage_pct'=0.36%;90;98 'tbs_e1apwork_usage'=114MB;28800;31360;0;32000 'tbs_e1apwork_alloc'=1981MB;;;0;32000 'tbs_e1aplarge_usage_pct'=0.32%;90;98 'tbs_e1aplarge_usage'=101MB;28800;31360;0;32000 'tbs_e1aplarge_alloc'=111MB;;;0;32000 'tbs_e1amwork_usage_pct'=0.03%;90;98 'tbs_e1amwork_usage'=8MB;28800;31360;0;32000 'tbs_e1amwork_alloc'=12MB;;;0;32000 'tbs_e1amlarge_usage_pct'=0.27%;90;98 'tbs_e1amlarge_usage'=85MB;28800;31360;0;32000 'tbs_e1amlarge_alloc'=90MB;;;0;32000 'tbs_dpwork_usage_pct'=0.00%;90;98 'tbs_dpwork_usage'=0MB;28800;31360;0;32000 'tbs_dpwork_alloc'=5MB;;;0;32000 'tbs_dplarge_usage_pct'=0.04%;90;98 'tbs_dplarge_usage'=12MB;28800;31360;0;32000 'tbs_dplarge_alloc'=14MB;;;0;32000 'tbs_dpapp_usage_pct'=0.09%;90;98 'tbs_dpapp_usage'=28MB;28800;31360;0;32000 'tbs_dpapp_alloc'=30MB;;;0;32000 'tbs_diapp_usage_pct'=0.00%;90;98 'tbs_diapp_usage'=0MB;28800;31360;0;32000 'tbs_diapp_alloc'=5MB;;;0;32000 'tbs_cularge_usage_pct'=0.01%;90;98 'tbs_cularge_usage'=2MB;28800;31360;0;32000 'tbs_cularge_alloc'=4MB;;;0;32000 'tbs_cularg3_usage_pct'=0.00%;90;98 'tbs_cularg3_usage'=0MB;28800;31360;0;32000 'tbs_cularg3_alloc'=2MB;;;0;32000 'tbs_cularg2_usage_pct'=0.00%;90;98 'tbs_cularg2_usage'=0MB;28800;31360;0;32000 'tbs_cularg2_alloc'=5MB;;;0;32000 'tbs_cularg1_usage_pct'=0.00%;90;98 'tbs_cularg1_usage'=0MB;28800;31360;0;32000 'tbs_cularg1_alloc'=5MB;;;0;32000 'tbs_cuaudit_usage_pct'=0.00%;90;98 'tbs_cuaudit_usage'=0MB;28800;31360;0;32000 'tbs_cuaudit_alloc'=5MB;;;0;32000 'tbs_cswork_usage_pct'=0.00%;90;98 'tbs_cswork_usage'=1MB;28800;31360;0;32000 'tbs_cswork_alloc'=3MB;;;0;32000 'tbs_cslarge_usage_pct'=0.01%;90;98 'tbs_cslarge_usage'=3MB;28800;31360;0;32000 'tbs_cslarge_alloc'=4MB;;;0;32000 'tbs_csapp_usage_pct'=0.03%;90;98 'tbs_csapp_usage'=8MB;28800;31360;0;32000 'tbs_csapp_alloc'=9MB;;;0;32000 'tbs_cplarge_usage_pct'=0.00%;90;98 'tbs_cplarge_usage'=0MB;28800;31360;0;32000 'tbs_cplarge_alloc'=5MB;;;0;32000 'tbs_cpapp_usage_pct'=0.02%;90;98 'tbs_cpapp_usage'=4MB;28800;31360;0;32000 'tbs_cpapp_alloc'=6MB;;;0;32000 'tbs_cfgapp_usage_pct'=0.00%;90;98 'tbs_cfgapp_usage'=0MB;28800;31360;0;32000 'tbs_cfgapp_alloc'=5MB;;;0;32000 'tbs_cawork_usage_pct'=0.15%;90;98 'tbs_cawork_usage'=49MB;28800;31360;0;32000 'tbs_cawork_alloc'=52MB;;;0;32000 'tbs_caapp_usage_pct'=0.20%;90;98 'tbs_caapp_usage'=65MB;28800;31360;0;32000 'tbs_caapp_alloc'=69MB;;;0;32000 'tbs_biwork_usage_pct'=0.37%;90;98 'tbs_biwork_usage'=119MB;28800;31360;0;32000 'tbs_biwork_alloc'=210MB;;;0;32000 'tbs_bilarge_usage_pct'=1.02%;90;98 'tbs_bilarge_usage'=324MB;28800;31360;0;32000 'tbs_bilarge_alloc'=345MB;;;0;32000 'tbs_biapp_usage_pct'=1.04%;90;98 'tbs_biapp_usage'=332MB;28800;31360;0;32000 'tbs_biapp_alloc'=343MB;;;0;32000 'tbs_bdlarge_usage_pct'=0.00%;90;98 'tbs_bdlarge_usage'=1MB;28800;31360;0;32000 'tbs_bdlarge_alloc'=2MB;;;0;32000 'tbs_bdapp_usage_pct'=0.04%;90;98 'tbs_bdapp_usage'=13MB;28800;31360;0;32000 'tbs_bdapp_alloc'=14MB;;;0;32000 'tbs_bcapp_usage_pct'=0.00%;90;98 'tbs_bcapp_usage'=0MB;28800;31360;0;32000 'tbs_bcapp_alloc'=5MB;;;0;32000 'tbs_auclrge_usage_pct'=0.00%;90;98 'tbs_auclrge_usage'=0MB;28800;31360;0;32000 'tbs_auclrge_alloc'=5MB;;;0;32000 'tbs_aucapp_usage_pct'=0.07%;90;98 'tbs_aucapp_usage'=23MB;28800;31360;0;32000 'tbs_aucapp_alloc'=25MB;;;0;32000 'tbs_arwork_usage_pct'=0.85%;90;98 'tbs_arwork_usage'=272MB;28800;31360;0;32000 'tbs_arwork_alloc'=364MB;;;0;32000 'tbs_arlarge_usage_pct'=2.52%;90;98 'tbs_arlarge_usage'=807MB;28800;31360;0;32000 'tbs_arlarge_alloc'=846MB;;;0;32000 'tbs_ararch_usage_pct'=0.01%;90;98 'tbs_ararch_usage'=2MB;28800;31360;0;32000 'tbs_ararch_alloc'=3MB;;;0;32000 'tbs_arapp_usage_pct'=0.30%;90;98 'tbs_arapp_usage'=96MB;28800;31360;0;32000 'tbs_arapp_alloc'=129MB;;;0;32000 'tbs_apwork_usage_pct'=0.84%;90;98 'tbs_apwork_usage'=269MB;28800;31360;0;32000 'tbs_apwork_alloc'=619MB;;;0;32000 'tbs_aplarge_usage_pct'=4.51%;90;98 'tbs_aplarge_usage'=2885MB;57600;62720;0;64000 'tbs_aplarge_alloc'=3000MB;;;0;64000 'tbs_aparch_usage_pct'=0.01%;90;98 'tbs_aparch_usage'=3MB;28800;31360;0;32000 'tbs_aparch_alloc'=5MB;;;0;32000 'tbs_apapp_usage_pct'=1.45%;90;98 'tbs_apapp_usage'=465MB;28800;31360;0;32000 'tbs_apapp_alloc'=551MB;;;0;32000 'tbs_amwork_usage_pct'=0.29%;90;98 'tbs_amwork_usage'=91MB;28800;31360;0;32000 'tbs_amwork_alloc'=97MB;;;0;32000 'tbs_amlarge_usage_pct'=0.77%;90;98 'tbs_amlarge_usage'=247MB;28800;31360;0;32000 'tbs_amlarge_alloc'=255MB;;;0;32000 'tbs_amarch_usage_pct'=0.00%;90;98 'tbs_amarch_usage'=0MB;28800;31360;0;32000 'tbs_amarch_alloc'=2MB;;;0;32000 'tbs_amapp_usage_pct'=0.74%;90;98 'tbs_amapp_usage'=236MB;28800;31360;0;32000 'tbs_amapp_alloc'=309MB;;;0;32000";

	return ds;
}

void simu_send_event(float interval, int nb, nebstruct_service_check_data check){
	printf("Send %i events\n", nb);
	int i;
	for (i=0; i<nb; i++) {
		printf(" + %i\n", i);
		service_check_data(NEBCALLBACK_SERVICE_CHECK_DATA, (void *)&check);
		sleep(interval);
	}
}

int main (void)
{
	int result = 0;
    char *error = NULL;
    void *func;

	char * args = "name=Debug 127.0.0.1 flush=10 debug=1 cache_file=/tmp/simu.cache";
	char * filename = "./neb2amqp.o";
	int should_be_loaded = 1;
	
	nebmodule *module=NULL;
	module=(nebmodule *)malloc(sizeof(nebmodule));
	
	printf("Init nebmodule struct\n");
	
	module->filename=(char *)strdup(filename);
	module->args=(args==NULL)?NULL:(char *)strdup(args);
	module->should_be_loaded=should_be_loaded;
	module->is_currently_loaded=FALSE;
	module->module_handle=NULL;
	module->init_func=NULL;
	module->deinit_func=NULL;

	printf("Load module ...\n");
    module->module_handle = dlopen (filename, RTLD_NOW);
    error = dlerror ();
    if (error) {
        printf (" + Error: %s\n", error);
        return ERROR;
    }
	printf(" + Ok\n");
	
	printf("Load init function...\n");
    func = dlsym (module->module_handle,"nebmodule_init");
    error = dlerror ();
    if (error) {
        printf (" + Error: %s\n", error);
        dlclose (module->module_handle);
        return ERROR;
    }
    memcpy (&(module->init_func), &func, sizeof(module->init_func));
	printf(" + Ok\n");
	
	printf("Load deinit function...\n");
    func = dlsym (module->module_handle,"nebmodule_deinit");
    error = dlerror ();
    if (error) {
        printf (" + Error: %s\n", error);
        dlclose (module->module_handle);
        return ERROR;
    }
    memcpy (&(module->deinit_func), &func, sizeof(module->init_func));
	printf(" + Ok\n");

	printf("Load toggle_blackout...\n");
    func = dlsym (module->module_handle,"toggle_blackout");
    error = dlerror ();
    if (error) {
        printf (" + Error: %s\n", error);
        dlclose (module->module_handle);
        return ERROR;
    }
    memcpy (&(toggle_blackout), &func, sizeof(toggle_blackout));
	printf(" + Ok\n");


	printf("\nInit NEB ...\n");
	int (*initfunc)(int,char *,void *);
	initfunc=module->init_func;
	result=(*initfunc)(NEBMODULE_NORMAL_LOAD, module->args, module->module_handle);

	sleep(1);
	/// Do it !
	printf("\nPlay with NEB ...\n");
	

	nebstruct_service_check_data check1;
	check1 = build_service_check("host1", "service1", "mon output1");
	
	/* ############################# START ############################## */

	simu_send_event(1.0, 5, check1);

	sleep(1);

	toggle_blackout();
	simu_send_event(0.1, 10, check1);
	toggle_blackout();


	sleep(1);
	simu_send_event(1.0, 20, check1);

	/* ############################# END ############################## */

	printf("\ndeInit NEB ...\n");
	int (*deinitfunc)(int, int);
	deinitfunc=module->deinit_func;
	result=(*deinitfunc)(0, NEBMODULE_FORCE_UNLOAD);

	printf("Unload module\n");
    result = dlclose (module->module_handle);
    if (result != 0) {
        printf (" + ERROR\n");
    } else {
    	printf(" + Ok\n");
    }

	printf("Free ...\n");
	free(module->filename);
	free(module->args);
	free(module);
	printf(" + Ok\n");
	
	printf("Bye Bye\n");
	return 0;
}
