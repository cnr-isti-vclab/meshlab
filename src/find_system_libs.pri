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


    #### header only libs ####
    #eigen
    exists(/usr/include/eigen3){
        CONFIG += system_eigen3
    }

    #glew
    exists(/usr/include/GL/glew.h){
        CONFIG += system_glew
    }
}
