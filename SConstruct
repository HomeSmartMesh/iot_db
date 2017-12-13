# - Glob() adds all the files with the extension to the list
# - CPPPATH define the path where to look for source files which have include dependendcies
#		so that when these include files change, a recompilation of the source file is triggered
#		CPPPATH = ['include', '/home/project/inc']



env = Environment()
env.Program(    target='iot_db',
                source=[Glob('*.cpp'),
                            [   'pilibs/utils.cpp',
                                'pilibs/safe_msg.cpp',
                                'pilibs/mqtt_db.cpp',
                                'pilibs/webserver.cpp',
                                'pilibs/db_mgr.cpp',
                                'pilibs/log.cpp'
                            ],
                        ],
                CPPPATH = ['.','pilibs/','/usr/local/include/Poco/'],
                CCFLAGS = ['-std=c++11'],
				LIBS=['libmosquittopp','libPocoNet','libPocoUtil','libPocoFoundation'],
				LIBPATH='/usr/local/lib/',
				LINKFLAGS=['-lboost_system','-lboost_filesystem']
            )
