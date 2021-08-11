PACKAGES=libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata bios
PACKAGE_DIRS=/home/juan/install_litex/litex/litex/soc/software/libcompiler_rt /home/juan/install_litex/litex/litex/soc/software/libbase /home/juan/install_litex/litex/litex/soc/software/libfatfs /home/juan/install_litex/litex/litex/soc/software/liblitespi /home/juan/install_litex/litex/litex/soc/software/liblitedram /home/juan/install_litex/litex/litex/soc/software/libliteeth /home/juan/install_litex/litex/litex/soc/software/liblitesdcard /home/juan/install_litex/litex/litex/soc/software/liblitesata /home/juan/install_litex/litex/litex/soc/software/bios
LIBS=libcompiler_rt libbase-nofloat libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata
TRIPLE=riscv64-unknown-elf
CPU=picorv32
CPUFLAGS=-mno-save-restore -march=rv32im     -mabi=ilp32 -D__picorv32__ 
CPUENDIANNESS=little
CLANG=0
CPU_DIRECTORY=/home/juan/install_litex/litex/litex/soc/cores/cpu/picorv32
SOC_DIRECTORY=/home/juan/install_litex/litex/litex/soc
COMPILER_RT_DIRECTORY=/home/juan/install_litex/pythondata-software-compiler_rt/pythondata_software_compiler_rt/data
export BUILDINC_DIRECTORY
BUILDINC_DIRECTORY=/home/juan/Documentos/Digital_II/Nexys4/Proyectos/Proyecto_Final/w07_entrega-_final-grupo11/build/nexys4ddr/software/include
LIBCOMPILER_RT_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/libcompiler_rt
LIBBASE_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/libbase
LIBFATFS_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/libfatfs
LIBLITESPI_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/liblitespi
LIBLITEDRAM_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/liblitedram
LIBLITEETH_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/libliteeth
LIBLITESDCARD_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/liblitesdcard
LIBLITESATA_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/liblitesata
BIOS_DIRECTORY=/home/juan/install_litex/litex/litex/soc/software/bios