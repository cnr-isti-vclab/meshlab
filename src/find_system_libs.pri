# this pri file finds all the external libraries already installed in the system
# that could be used instead of the ones placed in the external folder.

# to avoid to use system libraries and to use only the ones in the external folder,
# execute qmake with the following argument:
# qmake "CONFIG+=disable_system_libs"
!disable_system_libs {

linux {
    #### static libs ####

    # possible paths to look for static libs:
    # /usr/lib/
    # /usr/lib/i386-linux-gnu/
    # /usr/lib/x86_64-linux-gnu/
    #

    #openctm
    exists(/usr/lib/x86_64-linux-gnu/libopenctm.so){
        CONFIG += system_openctm
    }

    #lib3ds
    exists(/usr/lib/x86_64-linux-gnu/lib3ds.so){
        CONFIG += system_lib3ds
    }

    #muparser #cannot due to _UNICODE define
    #exists(/usr/lib/x86_64-linux-gnu/libmuparser.so){
    #    CONFIG += system_muparser
    #}

    #qhull
    exists(/usr/lib/x86_64-linux-gnu/libqhull.so){
        CONFIG += system_qhull
    }


    #### header only libs ####
    #eigen
    exists(/usr/include/eigen3){
        CONFIG += system_eigen3
    }
}

}
