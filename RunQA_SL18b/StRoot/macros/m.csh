root4star -l << EOF
    gSystem->Load("getRunBeginTime_C.so");
    gSystem->Load("getRunDuration_C.so");
    const char* database = "mysql://db04.star.bnl.gov:3414/RunLog?timeout=60";
    const char* user = "djs232";
    const char* pass = "";

    TMySQLServer* mysql = TMySQLServer::Connect(database,user,pass);

    string time = getRunBeginTime(mysql,  16128001);
    int    dur  = getRunDuration( mysql,  16128001);

    cout << " Begin time " << time << endl;
    cout << " duration:  " << dur << endl;

EOF
