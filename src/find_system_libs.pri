linux {
    # possible paths to look for static libs:
    # /usr/lib/
    # /usr/lib/i386-linux-gnu/
    # /usr/lib/x86_64-linux-gnu/
    #

    #openctm
    exists(/usr/lib/x86_64-linux-gnu/libopenctm.so){
        CONFIG += system_openctm
    }
}
