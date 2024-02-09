#!/bin/bash
#./dump_register_values.sh -S shm-b084-sp17-2 -N 2 -c cpu-b084-sp17 -y /afs/slac/g/controls/development/users/skoufis/other/CpswUtils/CpswUtils-git/AmcCarrierGmdDDV2_project.yaml/000TopLevel.yaml 
#./dump_register_values.sh -S shm-b084-sp18-1 -N 3 -c cpu-b084-sp18 -y /nfs/slac/g/lcls/epics/ioc/data/sioc-b084-ts03/yaml/000TopLevel.yaml
#./test_cpsw.sh -S shm-b084-sp18-1 -N 3 -c cpu-b084-sp18 -y /afs/slac/g/controls/development/users/skoufis/other/CpswUtils/CpswUtils-git/sp18_slot3_fw_yaml/000TopLevel.yaml
#./test_cpsw.sh -S shm-b084-sp18-1 -N 5 -c cpu-b084-sp18 -y /nfs/slac/g/lcls/epics/ioc/data/sioc-b084-ts05/yaml/000TopLevel.yaml -d /nfs/slac/g/lcls/epics/ioc/data/sioc-b084-ts05/yaml/
./dump_register_values.sh -S shm-b084-sp18-1 -N 5 -c cpu-b084-sp18 -y /nfs/slac/g/lcls/epics/ioc/data/sioc-b084-ts05/yaml/000TopLevel.yaml -d /nfs/slac/g/lcls/epics/ioc/data/sioc-b084-ts05/yaml/ 
