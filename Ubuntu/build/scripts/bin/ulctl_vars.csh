# ulctl_vars.csh - set environment variables

# Underlight env vars - generally speaking, don't change these
setenv UL_HOME    $HOME/lyra
setenv UL_PROD    ${UL_HOME}
setenv UL_BINDIR  ${UL_PROD}/bin
setenv UL_SBINDIR ${UL_PROD}/sbin
setenv UL_VARDIR  ${UL_PROD}/var
setenv UL_LOGDIR  ${UL_VARDIR}/log
setenv UL_OLOGDIR ${UL_LOGDIR}/old
setenv UL_DUMPDIR ${UL_VARDIR}/dump
setenv UL_PIDDIR  ${UL_VARDIR}/pid
setenv UL_VDBDIR  ${UL_VARDIR}/db
setenv UL_LSDIR   ${UL_VDBDIR}/level

# system env vars

# shared library path
setenv LD_LIBRARY_PATH ${UL_PROD}/lib:${UL_HOME}/lib

