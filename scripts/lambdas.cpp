vm.registerExternalFunction("print", [](ZenLoad::DaedalusVM& vm){
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
 
});

vm.registerExternalFunction("printmulti", [](ZenLoad::DaedalusVM& vm){
	std::string s4 = vm.popString(); LogInfo() << "s4: " << s4;
	std::string s3 = vm.popString(); LogInfo() << "s3: " << s3;
	std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
	std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
 
});

vm.registerExternalFunction("printdebug", [](ZenLoad::DaedalusVM& vm){
	std::string s = vm.popString(); LogInfo() << "s: " << s;
 
});

vm.registerExternalFunction("printscreen", [](ZenLoad::DaedalusVM& vm){
	int32_t timesec = vm.popDataValue(); LogInfo() << "timesec: " << timesec;
	std::string font = vm.popString(); LogInfo() << "font: " << font;
	int32_t posy = vm.popDataValue(); LogInfo() << "posy: " << posy;
	int32_t posx = vm.popDataValue(); LogInfo() << "posx: " << posx;
	std::string msg = vm.popString(); LogInfo() << "msg: " << msg;
	int32_t dialognr = vm.popDataValue(); LogInfo() << "dialognr: " << dialognr;
 
});

vm.registerExternalFunction("hlp_random", [](ZenLoad::DaedalusVM& vm){
	int32_t n0 = vm.popDataValue(); LogInfo() << "n0: " << n0;
 	vm.setReturn(0);
});

vm.registerExternalFunction("hlp_strcmp", [](ZenLoad::DaedalusVM& vm){
	std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
	std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("hlp_isvalidnpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("hlp_isvaliditem", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturn(0);
});

vm.registerExternalFunction("hlp_isitem", [](ZenLoad::DaedalusVM& vm){
	int32_t instancename = vm.popDataValue(); LogInfo() << "instancename: " << instancename;

 	vm.setReturn(0);
});

vm.registerExternalFunction("hlp_getnpc", [](ZenLoad::DaedalusVM& vm){
	int32_t instancename = vm.popDataValue(); LogInfo() << "instancename: " << instancename;
 	vm.setReturnVar(0);
});

vm.registerExternalFunction("hlp_getinstanceid", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("hlp_getinstanceid", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_wait", [](ZenLoad::DaedalusVM& vm){
	float n1 = vm.popFloatValue(); LogInfo() << "n1: " << n1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_getstatetime", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_setstatetime", [](ZenLoad::DaedalusVM& vm){
	int32_t seconds = vm.popDataValue(); LogInfo() << "seconds: " << seconds;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("wld_istime", [](ZenLoad::DaedalusVM& vm){
	int32_t min2 = vm.popDataValue(); LogInfo() << "min2: " << min2;
	int32_t hour2 = vm.popDataValue(); LogInfo() << "hour2: " << hour2;
	int32_t min1 = vm.popDataValue(); LogInfo() << "min1: " << min1;
	int32_t hour1 = vm.popDataValue(); LogInfo() << "hour1: " << hour1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_insertnpc", [](ZenLoad::DaedalusVM& vm){
	std::string spawnpoint = vm.popString(); LogInfo() << "spawnpoint: " << spawnpoint;
	int32_t npcinstance = vm.popDataValue(); LogInfo() << "npcinstance: " << npcinstance;
 
});

vm.registerExternalFunction("wld_insertnpcandrespawn", [](ZenLoad::DaedalusVM& vm){
	float spawndelay = vm.popFloatValue(); LogInfo() << "spawndelay: " << spawndelay;
	std::string spawnpoint = vm.popString(); LogInfo() << "spawnpoint: " << spawnpoint;
	int32_t instance = vm.popDataValue(); LogInfo() << "instance: " << instance;
 
});

vm.registerExternalFunction("ai_playani", [](ZenLoad::DaedalusVM& vm){
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_standup", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_standupquick", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_quicklook", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_lookat", [](ZenLoad::DaedalusVM& vm){
	std::string name = vm.popString(); LogInfo() << "name: " << name;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_lookatnpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_stoplookat", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_pointat", [](ZenLoad::DaedalusVM& vm){
	std::string name = vm.popString(); LogInfo() << "name: " << name;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_pointatnpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_stoppointat", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_takeitem", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_dropitem", [](ZenLoad::DaedalusVM& vm){
	int32_t itemid = vm.popDataValue(); LogInfo() << "itemid: " << itemid;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_useitem", [](ZenLoad::DaedalusVM& vm){
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_useitemtostate", [](ZenLoad::DaedalusVM& vm){
	int32_t state = vm.popDataValue(); LogInfo() << "state: " << state;
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_usemob", [](ZenLoad::DaedalusVM& vm){
	int32_t targetstate = vm.popDataValue(); LogInfo() << "targetstate: " << targetstate;
	std::string schemename = vm.popString(); LogInfo() << "schemename: " << schemename;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_ismobavailable", [](ZenLoad::DaedalusVM& vm){
	std::string schemename = vm.popString(); LogInfo() << "schemename: " << schemename;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_getmobstate", [](ZenLoad::DaedalusVM& vm){
	std::string schemename = vm.popString(); LogInfo() << "schemename: " << schemename;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_setwalkmode", [](ZenLoad::DaedalusVM& vm){
	int32_t n0 = vm.popDataValue(); LogInfo() << "n0: " << n0;
	uint32_t arr_n;
	int32_t n = vm.popVar(arr_n); LogInfo() << "n: " << n;
 
});

vm.registerExternalFunction("ai_gotowp", [](ZenLoad::DaedalusVM& vm){
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_gotofp", [](ZenLoad::DaedalusVM& vm){
	std::string fpname = vm.popString(); LogInfo() << "fpname: " << fpname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_gotonextfp", [](ZenLoad::DaedalusVM& vm){
	std::string fpname = vm.popString(); LogInfo() << "fpname: " << fpname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_gotonpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_gotoitem", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_gotosound", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_teleport", [](ZenLoad::DaedalusVM& vm){
	std::string waypoint = vm.popString(); LogInfo() << "waypoint: " << waypoint;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_getnearestwp", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn("");
});

vm.registerExternalFunction("npc_getnextwp", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn("");
});

vm.registerExternalFunction("wld_isfpavailable", [](ZenLoad::DaedalusVM& vm){
	std::string fpname = vm.popString(); LogInfo() << "fpname: " << fpname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_isnextfpavailable", [](ZenLoad::DaedalusVM& vm){
	std::string fpname = vm.popString(); LogInfo() << "fpname: " << fpname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isonfp", [](ZenLoad::DaedalusVM& vm){
	std::string name = vm.popString(); LogInfo() << "name: " << name;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_iswayblocked", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_turntonpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n1;
	int32_t n1 = vm.popVar(arr_n1); LogInfo() << "n1: " << n1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_turnaway", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n1;
	int32_t n1 = vm.popVar(arr_n1); LogInfo() << "n1: " << n1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_whirlaround", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_turntosound", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_aligntowp", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_dodge", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 
});

vm.registerExternalFunction("mdl_applyoverlaymds", [](ZenLoad::DaedalusVM& vm){
	std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("mdl_removeoverlaymds", [](ZenLoad::DaedalusVM& vm){
	std::string overlayname = vm.popString(); LogInfo() << "overlayname: " << overlayname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("mdl_applyoverlaymdstimed", [](ZenLoad::DaedalusVM& vm){
	float timeticks = vm.popFloatValue(); LogInfo() << "timeticks: " << timeticks;
	std::string overlayname = vm.popString(); LogInfo() << "overlayname: " << overlayname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("mdl_applyrandomani", [](ZenLoad::DaedalusVM& vm){
	std::string s2 = vm.popString(); LogInfo() << "s2: " << s2;
	std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("mdl_applyrandomanifreq", [](ZenLoad::DaedalusVM& vm){
	float f2 = vm.popFloatValue(); LogInfo() << "f2: " << f2;
	std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("mdl_startfaceani", [](ZenLoad::DaedalusVM& vm){
	float holdtime = vm.popFloatValue(); LogInfo() << "holdtime: " << holdtime;
	float intensity = vm.popFloatValue(); LogInfo() << "intensity: " << intensity;
	std::string name = vm.popString(); LogInfo() << "name: " << name;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("mdl_applyrandomfaceani", [](ZenLoad::DaedalusVM& vm){
	float probmin = vm.popFloatValue(); LogInfo() << "probmin: " << probmin;
	float timemaxvar = vm.popFloatValue(); LogInfo() << "timemaxvar: " << timemaxvar;
	float timemax = vm.popFloatValue(); LogInfo() << "timemax: " << timemax;
	float timeminvar = vm.popFloatValue(); LogInfo() << "timeminvar: " << timeminvar;
	float timemin = vm.popFloatValue(); LogInfo() << "timemin: " << timemin;
	std::string name = vm.popString(); LogInfo() << "name: " << name;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_getbodystate", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasbodyflag", [](ZenLoad::DaedalusVM& vm){
	int32_t bodyflag = vm.popDataValue(); LogInfo() << "bodyflag: " << bodyflag;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_playanibs", [](ZenLoad::DaedalusVM& vm){
	int32_t bodystate = vm.popDataValue(); LogInfo() << "bodystate: " << bodystate;
	std::string aniname = vm.popString(); LogInfo() << "aniname: " << aniname;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 
});

vm.registerExternalFunction("npc_settofistmode", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_settofightmode", [](ZenLoad::DaedalusVM& vm){
	int32_t weapon = vm.popDataValue(); LogInfo() << "weapon: " << weapon;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_isinfightmode", [](ZenLoad::DaedalusVM& vm){
	int32_t fmode = vm.popDataValue(); LogInfo() << "fmode: " << fmode;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_drawweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_readymeleeweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_readyrangedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_removeweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_getreadiedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_hasreadiedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasreadiedmeleeweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasreadiedrangedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasrangedweaponwithammo", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_gettarget", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getnexttarget", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isnexttargetavailable", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_settarget", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_attack", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_finishingmove", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_defend", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_flee", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_aimat", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_target;
	int32_t target = vm.popVar(arr_target); LogInfo() << "target: " << target;
	uint32_t arr_attacker;
	int32_t attacker = vm.popVar(arr_attacker); LogInfo() << "attacker: " << attacker;
 
});

vm.registerExternalFunction("ai_shootat", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_target;
	int32_t target = vm.popVar(arr_target); LogInfo() << "target: " << target;
	uint32_t arr_attacker;
	int32_t attacker = vm.popVar(arr_attacker); LogInfo() << "attacker: " << attacker;
 
});

vm.registerExternalFunction("ai_stopaim", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_attacker;
	int32_t attacker = vm.popVar(arr_attacker); LogInfo() << "attacker: " << attacker;
 
});

vm.registerExternalFunction("npc_arewestronger", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isaiming", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_insertitem", [](ZenLoad::DaedalusVM& vm){
	std::string spawnpoint = vm.popString(); LogInfo() << "spawnpoint: " << spawnpoint;
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
 
});

vm.registerExternalFunction("ai_lookforitem", [](ZenLoad::DaedalusVM& vm){
	int32_t instance = vm.popDataValue(); LogInfo() << "instance: " << instance;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("wld_removeitem", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturn(0);
});

vm.registerExternalFunction("createinvitem", [](ZenLoad::DaedalusVM& vm){
	int32_t n1 = vm.popDataValue(); LogInfo() << "n1: " << n1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("createinvitems", [](ZenLoad::DaedalusVM& vm){
	int32_t n2 = vm.popDataValue(); LogInfo() << "n2: " << n2;
	int32_t n1 = vm.popDataValue(); LogInfo() << "n1: " << n1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_getinvitem", [](ZenLoad::DaedalusVM& vm){
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_hasitems", [](ZenLoad::DaedalusVM& vm){
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getinvitembyslot", [](ZenLoad::DaedalusVM& vm){
	int32_t slotnr = vm.popDataValue(); LogInfo() << "slotnr: " << slotnr;
	int32_t category = vm.popDataValue(); LogInfo() << "category: " << category;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_removeinvitem", [](ZenLoad::DaedalusVM& vm){
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_owner;
	int32_t owner = vm.popVar(arr_owner); LogInfo() << "owner: " << owner;
 
});

vm.registerExternalFunction("npc_removeinvitems", [](ZenLoad::DaedalusVM& vm){
	int32_t amount = vm.popDataValue(); LogInfo() << "amount: " << amount;
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_owner;
	int32_t owner = vm.popVar(arr_owner); LogInfo() << "owner: " << owner;
 
});

vm.registerExternalFunction("mob_createitems", [](ZenLoad::DaedalusVM& vm){
	int32_t amount = vm.popDataValue(); LogInfo() << "amount: " << amount;
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	std::string mobname = vm.popString(); LogInfo() << "mobname: " << mobname;
 
});

vm.registerExternalFunction("mob_hasitems", [](ZenLoad::DaedalusVM& vm){
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	std::string mobname = vm.popString(); LogInfo() << "mobname: " << mobname;
 	vm.setReturn(0);
});

vm.registerExternalFunction("equipitem", [](ZenLoad::DaedalusVM& vm){
	int32_t n1 = vm.popDataValue(); LogInfo() << "n1: " << n1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("ai_equipbestmeleeweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_equipbestrangedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_equipbestarmor", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_unequipweapons", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_unequiparmor", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_equiparmor", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_owner;
	int32_t owner = vm.popVar(arr_owner); LogInfo() << "owner: " << owner;
 
});

vm.registerExternalFunction("npc_getequippedmeleeweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_getequippedrangedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_getequippedarmor", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_hasequippedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasequippedmeleeweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasequippedrangedweapon", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasequippedarmor", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_ownedbynpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;

 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_ownedbyguild", [](ZenLoad::DaedalusVM& vm){
	int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;

 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isdetectedmobownedbynpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_owner;
	int32_t owner = vm.popVar(arr_owner); LogInfo() << "owner: " << owner;
	uint32_t arr_user;
	int32_t user = vm.popVar(arr_user); LogInfo() << "user: " << user;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_giveitem", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n2;
	int32_t n2 = vm.popVar(arr_n2); LogInfo() << "n2: " << n2;

	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("npc_startitemreactmodules", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasoffered", [](ZenLoad::DaedalusVM& vm){
	int32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("doc_open", [](ZenLoad::DaedalusVM& vm){
	std::string picname = vm.popString(); LogInfo() << "picname: " << picname;
 
});

vm.registerExternalFunction("doc_font", [](ZenLoad::DaedalusVM& vm){
	std::string fontname = vm.popString(); LogInfo() << "fontname: " << fontname;
 
});

vm.registerExternalFunction("doc_print", [](ZenLoad::DaedalusVM& vm){
	std::string text = vm.popString(); LogInfo() << "text: " << text;
 
});

vm.registerExternalFunction("doc_mapcoordinates", [](ZenLoad::DaedalusVM& vm){
	float pixely2 = vm.popFloatValue(); LogInfo() << "pixely2: " << pixely2;
	float pixelx2 = vm.popFloatValue(); LogInfo() << "pixelx2: " << pixelx2;
	float gamey2 = vm.popFloatValue(); LogInfo() << "gamey2: " << gamey2;
	float gamex2 = vm.popFloatValue(); LogInfo() << "gamex2: " << gamex2;
	float pixely1 = vm.popFloatValue(); LogInfo() << "pixely1: " << pixely1;
	float pixelx1 = vm.popFloatValue(); LogInfo() << "pixelx1: " << pixelx1;
	float gamey1 = vm.popFloatValue(); LogInfo() << "gamey1: " << gamey1;
	float gamex1 = vm.popFloatValue(); LogInfo() << "gamex1: " << gamex1;
	std::string levelname = vm.popString(); LogInfo() << "levelname: " << levelname;
 
});

vm.registerExternalFunction("ai_output", [](ZenLoad::DaedalusVM& vm){
	std::string outputname = vm.popString(); LogInfo() << "outputname: " << outputname;
	uint32_t arr_target;
	int32_t target = vm.popVar(arr_target); LogInfo() << "target: " << target;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_outputsvm", [](ZenLoad::DaedalusVM& vm){
	std::string svmname = vm.popString(); LogInfo() << "svmname: " << svmname;
	uint32_t arr_target;
	int32_t target = vm.popVar(arr_target); LogInfo() << "target: " << target;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_outputsvm_overlay", [](ZenLoad::DaedalusVM& vm){
	std::string svmname = vm.popString(); LogInfo() << "svmname: " << svmname;
	uint32_t arr_target;
	int32_t target = vm.popVar(arr_target); LogInfo() << "target: " << target;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_waittillend", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_ask", [](ZenLoad::DaedalusVM& vm){


	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_asktext", [](ZenLoad::DaedalusVM& vm){
	std::string strno = vm.popString(); LogInfo() << "strno: " << strno;
	std::string stryes = vm.popString(); LogInfo() << "stryes: " << stryes;


	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_waitforquestion", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_setrefusetalk", [](ZenLoad::DaedalusVM& vm){
	int32_t timesec = vm.popDataValue(); LogInfo() << "timesec: " << timesec;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_refusetalk", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_memoryentry", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_victim;
	int32_t victim = vm.popVar(arr_victim); LogInfo() << "victim: " << victim;
	int32_t newsid = vm.popDataValue(); LogInfo() << "newsid: " << newsid;
	uint32_t arr_offender;
	int32_t offender = vm.popVar(arr_offender); LogInfo() << "offender: " << offender;
	int32_t source = vm.popDataValue(); LogInfo() << "source: " << source;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_memoryentryguild", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_victimguild;
	int32_t victimguild = vm.popVar(arr_victimguild); LogInfo() << "victimguild: " << victimguild;
	int32_t newsid = vm.popDataValue(); LogInfo() << "newsid: " << newsid;
	uint32_t arr_offender;
	int32_t offender = vm.popVar(arr_offender); LogInfo() << "offender: " << offender;
	int32_t source = vm.popDataValue(); LogInfo() << "source: " << source;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_hasnews", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_victim;
	int32_t victim = vm.popVar(arr_victim); LogInfo() << "victim: " << victim;
	uint32_t arr_offender;
	int32_t offender = vm.popVar(arr_offender); LogInfo() << "offender: " << offender;
	int32_t newsid = vm.popDataValue(); LogInfo() << "newsid: " << newsid;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isnewsgossip", [](ZenLoad::DaedalusVM& vm){
	int32_t newsnumber = vm.popDataValue(); LogInfo() << "newsnumber: " << newsnumber;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getnewswitness", [](ZenLoad::DaedalusVM& vm){
	int32_t newsnumber = vm.popDataValue(); LogInfo() << "newsnumber: " << newsnumber;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturnVar(0);
});

vm.registerExternalFunction("npc_getnewsvictim", [](ZenLoad::DaedalusVM& vm){
	int32_t newsnumber = vm.popDataValue(); LogInfo() << "newsnumber: " << newsnumber;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturnVar(0);
});

vm.registerExternalFunction("npc_getnewsoffender", [](ZenLoad::DaedalusVM& vm){
	int32_t newsnumber = vm.popDataValue(); LogInfo() << "newsnumber: " << newsnumber;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturnVar(0);
});

vm.registerExternalFunction("npc_isdead", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_knowsinfo", [](ZenLoad::DaedalusVM& vm){
	int32_t infoinstance = vm.popDataValue(); LogInfo() << "infoinstance: " << infoinstance;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_checkinfo", [](ZenLoad::DaedalusVM& vm){
	int32_t important = vm.popDataValue(); LogInfo() << "important: " << important;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_giveinfo", [](ZenLoad::DaedalusVM& vm){
	int32_t important = vm.popDataValue(); LogInfo() << "important: " << important;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_checkavailablemission", [](ZenLoad::DaedalusVM& vm){
	int32_t important = vm.popDataValue(); LogInfo() << "important: " << important;
	int32_t missionstate = vm.popDataValue(); LogInfo() << "missionstate: " << missionstate;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_checkrunningmission", [](ZenLoad::DaedalusVM& vm){
	int32_t important = vm.popDataValue(); LogInfo() << "important: " << important;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_checkoffermission", [](ZenLoad::DaedalusVM& vm){
	int32_t important = vm.popDataValue(); LogInfo() << "important: " << important;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("mis_setstatus", [](ZenLoad::DaedalusVM& vm){
	int32_t newstatus = vm.popDataValue(); LogInfo() << "newstatus: " << newstatus;
	int32_t missionname = vm.popDataValue(); LogInfo() << "missionname: " << missionname;
 
});

vm.registerExternalFunction("mis_getstatus", [](ZenLoad::DaedalusVM& vm){
	int32_t missionname = vm.popDataValue(); LogInfo() << "missionname: " << missionname;
 	vm.setReturn(0);
});

vm.registerExternalFunction("mis_ontime", [](ZenLoad::DaedalusVM& vm){
	int32_t missionname = vm.popDataValue(); LogInfo() << "missionname: " << missionname;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_stopprocessinfos", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 
});

vm.registerExternalFunction("npc_isplayer", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_player;
	int32_t player = vm.popVar(arr_player); LogInfo() << "player: " << player;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_detectplayer", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasdetectednpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isnear", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getdisttonpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc2;
	int32_t npc2 = vm.popVar(arr_npc2); LogInfo() << "npc2: " << npc2;
	uint32_t arr_npc1;
	int32_t npc1 = vm.popVar(arr_npc1); LogInfo() << "npc1: " << npc1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getdisttowp", [](ZenLoad::DaedalusVM& vm){
	std::string wpname = vm.popString(); LogInfo() << "wpname: " << wpname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getdisttoitem", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getdisttoplayer", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc1;
	int32_t npc1 = vm.popVar(arr_npc1); LogInfo() << "npc1: " << npc1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("snd_getdisttosource", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_gettrueguild", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_setattitude", [](ZenLoad::DaedalusVM& vm){
	int32_t att = vm.popDataValue(); LogInfo() << "att: " << att;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_settempattitude", [](ZenLoad::DaedalusVM& vm){
	int32_t att = vm.popDataValue(); LogInfo() << "att: " << att;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_getattitude", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_settrueguild", [](ZenLoad::DaedalusVM& vm){
	int32_t guildid = vm.popDataValue(); LogInfo() << "guildid: " << guildid;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_setguildattitude", [](ZenLoad::DaedalusVM& vm){
	int32_t guild2 = vm.popDataValue(); LogInfo() << "guild2: " << guild2;
	int32_t attitude = vm.popDataValue(); LogInfo() << "attitude: " << attitude;
	int32_t guild1 = vm.popDataValue(); LogInfo() << "guild1: " << guild1;
 
});

vm.registerExternalFunction("wld_getguildattitude", [](ZenLoad::DaedalusVM& vm){
	int32_t guild2 = vm.popDataValue(); LogInfo() << "guild2: " << guild2;
	int32_t guild1 = vm.popDataValue(); LogInfo() << "guild1: " << guild1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getpermattitude", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_exchangeguildattitudes", [](ZenLoad::DaedalusVM& vm){
	std::string name = vm.popString(); LogInfo() << "name: " << name;
 
});

vm.registerExternalFunction("npc_getguildattitude", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc_1;
	int32_t npc_1 = vm.popVar(arr_npc_1); LogInfo() << "npc_1: " << npc_1;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_setknowsplayer", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_player;
	int32_t player = vm.popVar(arr_player); LogInfo() << "player: " << player;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_knowsplayer", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_player;
	int32_t player = vm.popVar(arr_player); LogInfo() << "player: " << player;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_startstate", [](ZenLoad::DaedalusVM& vm){
	std::string wpname = vm.popString(); LogInfo() << "wpname: " << wpname;
	int32_t statebehaviour = vm.popDataValue(); LogInfo() << "statebehaviour: " << statebehaviour;

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_clearaiqueue", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_setnpcstostate", [](ZenLoad::DaedalusVM& vm){
	int32_t radius = vm.popDataValue(); LogInfo() << "radius: " << radius;

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_isinstate", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_wasinstate", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ta", [](ZenLoad::DaedalusVM& vm){
	std::string waypoint = vm.popString(); LogInfo() << "waypoint: " << waypoint;

	int32_t stop_h = vm.popDataValue(); LogInfo() << "stop_h: " << stop_h;
	int32_t start_h = vm.popDataValue(); LogInfo() << "start_h: " << start_h;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ta_min", [](ZenLoad::DaedalusVM& vm){
	std::string waypoint = vm.popString(); LogInfo() << "waypoint: " << waypoint;

	int32_t stop_m = vm.popDataValue(); LogInfo() << "stop_m: " << stop_m;
	int32_t stop_h = vm.popDataValue(); LogInfo() << "stop_h: " << stop_h;
	int32_t start_m = vm.popDataValue(); LogInfo() << "start_m: " << start_m;
	int32_t start_h = vm.popDataValue(); LogInfo() << "start_h: " << start_h;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_continueroutine", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_isinroutine", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_exchangeroutine", [](ZenLoad::DaedalusVM& vm){
	std::string routinename = vm.popString(); LogInfo() << "routinename: " << routinename;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("wld_setobjectroutine", [](ZenLoad::DaedalusVM& vm){
	int32_t state = vm.popDataValue(); LogInfo() << "state: " << state;
	std::string objname = vm.popString(); LogInfo() << "objname: " << objname;
	int32_t min1 = vm.popDataValue(); LogInfo() << "min1: " << min1;
	int32_t hour1 = vm.popDataValue(); LogInfo() << "hour1: " << hour1;
 
});

vm.registerExternalFunction("wld_setmobroutine", [](ZenLoad::DaedalusVM& vm){
	int32_t state = vm.popDataValue(); LogInfo() << "state: " << state;
	std::string objname = vm.popString(); LogInfo() << "objname: " << objname;
	int32_t min1 = vm.popDataValue(); LogInfo() << "min1: " << min1;
	int32_t hour1 = vm.popDataValue(); LogInfo() << "hour1: " << hour1;
 
});

vm.registerExternalFunction("rtn_exchange", [](ZenLoad::DaedalusVM& vm){
	std::string newroutine = vm.popString(); LogInfo() << "newroutine: " << newroutine;
	std::string oldroutine = vm.popString(); LogInfo() << "oldroutine: " << oldroutine;
 
});

vm.registerExternalFunction("ta_beginoverlay", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ta_endoverlay", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ta_removeoverlay", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("mdl_setmodelscale", [](ZenLoad::DaedalusVM& vm){
	float z = vm.popFloatValue(); LogInfo() << "z: " << z;
	float y = vm.popFloatValue(); LogInfo() << "y: " << y;
	float x = vm.popFloatValue(); LogInfo() << "x: " << x;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("mdl_setmodelfatness", [](ZenLoad::DaedalusVM& vm){
	float fatness = vm.popFloatValue(); LogInfo() << "fatness: " << fatness;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_changeattribute", [](ZenLoad::DaedalusVM& vm){
	int32_t value = vm.popDataValue(); LogInfo() << "value: " << value;
	int32_t atr = vm.popDataValue(); LogInfo() << "atr: " << atr;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_hastalent", [](ZenLoad::DaedalusVM& vm){
	int32_t tal = vm.popDataValue(); LogInfo() << "tal: " << tal;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_hasfighttalent", [](ZenLoad::DaedalusVM& vm){
	int32_t tal = vm.popDataValue(); LogInfo() << "tal: " << tal;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_createspell", [](ZenLoad::DaedalusVM& vm){
	int32_t spellnr = vm.popDataValue(); LogInfo() << "spellnr: " << spellnr;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_learnspell", [](ZenLoad::DaedalusVM& vm){
	int32_t spellnr = vm.popDataValue(); LogInfo() << "spellnr: " << spellnr;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_setteleportpos", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_getactivespell", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getactivespellcat", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_setactivespellinfo", [](ZenLoad::DaedalusVM& vm){
	int32_t i1 = vm.popDataValue(); LogInfo() << "i1: " << i1;
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getactivespelllevel", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ai_readyspell", [](ZenLoad::DaedalusVM& vm){
	int32_t investmana = vm.popDataValue(); LogInfo() << "investmana: " << investmana;
	int32_t spellid = vm.popDataValue(); LogInfo() << "spellid: " << spellid;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_unreadyspell", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_hasspell", [](ZenLoad::DaedalusVM& vm){
	int32_t spellid = vm.popDataValue(); LogInfo() << "spellid: " << spellid;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_percenable", [](ZenLoad::DaedalusVM& vm){

	int32_t percid = vm.popDataValue(); LogInfo() << "percid: " << percid;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_percdisable", [](ZenLoad::DaedalusVM& vm){
	int32_t percid = vm.popDataValue(); LogInfo() << "percid: " << percid;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_setperctime", [](ZenLoad::DaedalusVM& vm){
	float seconds = vm.popFloatValue(); LogInfo() << "seconds: " << seconds;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("perc_setrange", [](ZenLoad::DaedalusVM& vm){
	int32_t range = vm.popDataValue(); LogInfo() << "range: " << range;
	int32_t percid = vm.popDataValue(); LogInfo() << "percid: " << percid;
 
});

vm.registerExternalFunction("npc_sendpassiveperc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc3;
	int32_t npc3 = vm.popVar(arr_npc3); LogInfo() << "npc3: " << npc3;
	uint32_t arr_npc2;
	int32_t npc2 = vm.popVar(arr_npc2); LogInfo() << "npc2: " << npc2;
	int32_t perc_type = vm.popDataValue(); LogInfo() << "perc_type: " << perc_type;
	uint32_t arr_npc1;
	int32_t npc1 = vm.popVar(arr_npc1); LogInfo() << "npc1: " << npc1;
 
});

vm.registerExternalFunction("npc_sendsingleperc", [](ZenLoad::DaedalusVM& vm){
	int32_t percid = vm.popDataValue(); LogInfo() << "percid: " << percid;
	uint32_t arr_target;
	int32_t target = vm.popVar(arr_target); LogInfo() << "target: " << target;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("npc_perceiveall", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("wld_detectnpc", [](ZenLoad::DaedalusVM& vm){
	int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;

	int32_t instance = vm.popDataValue(); LogInfo() << "instance: " << instance;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_detectitem", [](ZenLoad::DaedalusVM& vm){
	int32_t flags = vm.popDataValue(); LogInfo() << "flags: " << flags;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_getdetectedmob", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn("");
});

vm.registerExternalFunction("npc_canseenpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc2;
	int32_t npc2 = vm.popVar(arr_npc2); LogInfo() << "npc2: " << npc2;
	uint32_t arr_npc1;
	int32_t npc1 = vm.popVar(arr_npc1); LogInfo() << "npc1: " << npc1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_canseenpcfreelos", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_other;
	int32_t other = vm.popVar(arr_other); LogInfo() << "other: " << other;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_canseeitem", [](ZenLoad::DaedalusVM& vm){

	uint32_t arr_npc1;
	int32_t npc1 = vm.popVar(arr_npc1); LogInfo() << "npc1: " << npc1;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_canseesource", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("ta_cs", [](ZenLoad::DaedalusVM& vm){
	std::string rolename = vm.popString(); LogInfo() << "rolename: " << rolename;
	std::string csname = vm.popString(); LogInfo() << "csname: " << csname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("ai_playcutscene", [](ZenLoad::DaedalusVM& vm){
	std::string csname = vm.popString(); LogInfo() << "csname: " << csname;
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 
});

vm.registerExternalFunction("hlp_cutsceneplayed", [](ZenLoad::DaedalusVM& vm){
	std::string csname = vm.popString(); LogInfo() << "csname: " << csname;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isincutscene", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("snd_play", [](ZenLoad::DaedalusVM& vm){
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
 
});

vm.registerExternalFunction("snd_play3d", [](ZenLoad::DaedalusVM& vm){
	std::string s1 = vm.popString(); LogInfo() << "s1: " << s1;
	uint32_t arr_n0;
	int32_t n0 = vm.popVar(arr_n0); LogInfo() << "n0: " << n0;
 
});

vm.registerExternalFunction("snd_issourcenpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("snd_issourceitem", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_self;
	int32_t self = vm.popVar(arr_self); LogInfo() << "self: " << self;
 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_assignroomtoguild", [](ZenLoad::DaedalusVM& vm){
	int32_t guild = vm.popDataValue(); LogInfo() << "guild: " << guild;
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
 
});

vm.registerExternalFunction("wld_assignroomtonpc", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_roomowner;
	int32_t roomowner = vm.popVar(arr_roomowner); LogInfo() << "roomowner: " << roomowner;
	std::string s0 = vm.popString(); LogInfo() << "s0: " << s0;
 
});

vm.registerExternalFunction("wld_getplayerportalowner", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturnVar(0);
});

vm.registerExternalFunction("wld_getplayerportalguild", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturn(0);
});

vm.registerExternalFunction("wld_getformerplayerportalowner", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturnVar(0);
});

vm.registerExternalFunction("wld_getformerplayerportalguild", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_isplayerinmyroom", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("npc_wasplayerinmyroom", [](ZenLoad::DaedalusVM& vm){
	uint32_t arr_npc;
	int32_t npc = vm.popVar(arr_npc); LogInfo() << "npc: " << npc;
 	vm.setReturn(0);
});

vm.registerExternalFunction("inttostring", [](ZenLoad::DaedalusVM& vm){
	int32_t x = vm.popDataValue(); LogInfo() << "x: " << x;
 	vm.setReturn("");
});

vm.registerExternalFunction("floattoint", [](ZenLoad::DaedalusVM& vm){
	float x = vm.popFloatValue(); LogInfo() << "x: " << x;
 	vm.setReturn(0);
});

vm.registerExternalFunction("inttofloat", [](ZenLoad::DaedalusVM& vm){
	int32_t x = vm.popDataValue(); LogInfo() << "x: " << x;
 	vm.setReturn(0.0f);
});

vm.registerExternalFunction("concatstrings", [](ZenLoad::DaedalusVM& vm){
	std::string str2 = vm.popString(); LogInfo() << "str2: " << str2;
	std::string str1 = vm.popString(); LogInfo() << "str1: " << str1;
 	vm.setReturn("");
});

vm.registerExternalFunction("printdebuginst", [](ZenLoad::DaedalusVM& vm){
	std::string text = vm.popString(); LogInfo() << "text: " << text;
 
});

vm.registerExternalFunction("printdebuginstch", [](ZenLoad::DaedalusVM& vm){
	std::string text = vm.popString(); LogInfo() << "text: " << text;
 
});

vm.registerExternalFunction("printdebugch", [](ZenLoad::DaedalusVM& vm){
	std::string text = vm.popString(); LogInfo() << "text: " << text;
 
});

vm.registerExternalFunction("log_createtopic", [](ZenLoad::DaedalusVM& vm){
	int32_t section = vm.popDataValue(); LogInfo() << "section: " << section;
	std::string name = vm.popString(); LogInfo() << "name: " << name;
 
});

vm.registerExternalFunction("log_settopicstatus", [](ZenLoad::DaedalusVM& vm){
	int32_t status = vm.popDataValue(); LogInfo() << "status: " << status;
	std::string name = vm.popString(); LogInfo() << "name: " << name;
 
});

vm.registerExternalFunction("log_addentry", [](ZenLoad::DaedalusVM& vm){
	std::string entry = vm.popString(); LogInfo() << "entry: " << entry;
	std::string topic = vm.popString(); LogInfo() << "topic: " << topic;
 
});

vm.registerExternalFunction("doc_create", [](ZenLoad::DaedalusVM& vm){

 	vm.setReturn(0);
});

vm.registerExternalFunction("doc_setpages", [](ZenLoad::DaedalusVM& vm){
	int32_t pages = vm.popDataValue(); LogInfo() << "pages: " << pages;
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

vm.registerExternalFunction("doc_setpage", [](ZenLoad::DaedalusVM& vm){
	int32_t vbool = vm.popDataValue(); LogInfo() << "vbool: " << vbool;
	std::string pageimage = vm.popString(); LogInfo() << "pageimage: " << pageimage;
	int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

vm.registerExternalFunction("doc_setfont", [](ZenLoad::DaedalusVM& vm){
	std::string fontname = vm.popString(); LogInfo() << "fontname: " << fontname;
	int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

vm.registerExternalFunction("doc_setmargins", [](ZenLoad::DaedalusVM& vm){
	int32_t vbool = vm.popDataValue(); LogInfo() << "vbool: " << vbool;
	int32_t bottommargin = vm.popDataValue(); LogInfo() << "bottommargin: " << bottommargin;
	int32_t rightmargin = vm.popDataValue(); LogInfo() << "rightmargin: " << rightmargin;
	int32_t topmargin = vm.popDataValue(); LogInfo() << "topmargin: " << topmargin;
	int32_t leftmargin = vm.popDataValue(); LogInfo() << "leftmargin: " << leftmargin;
	int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

vm.registerExternalFunction("doc_printline", [](ZenLoad::DaedalusVM& vm){
	std::string text = vm.popString(); LogInfo() << "text: " << text;
	int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

vm.registerExternalFunction("doc_printlines", [](ZenLoad::DaedalusVM& vm){
	std::string text = vm.popString(); LogInfo() << "text: " << text;
	int32_t page = vm.popDataValue(); LogInfo() << "page: " << page;
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

vm.registerExternalFunction("doc_show", [](ZenLoad::DaedalusVM& vm){
	int32_t handle = vm.popDataValue(); LogInfo() << "handle: " << handle;
 
});

