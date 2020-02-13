# This is an automatic qmake code that, if exist an xml file 
# *with the same name of the project*,
# it will copy it into the right directory
#
# You should have 'include (../../shared_post.pri)' at the end of each pro of a xml based plugin 

exists( $${_PRO_FILE_PWD_}/$${TARGET}.xml ) {

QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$$shell_path($${_PRO_FILE_PWD_}/$${TARGET}.xml)\" \"$$shell_path($$MESHLAB_DISTRIB_DIRECTORY/plugins/$${TARGET}.xml)\";

}

