# This is 

exists( $${_PRO_FILE_PWD_}/$${TARGET}.xml ) {

QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$${_PRO_FILE_PWD_}/$${TARGET}.xml\" \"../../distrib/plugins/$${TARGET}.xml\";

}

