-- 服务器ip
server_ip = "localhost"
-- 服务器端口
server_port = 23543

-- 配置文件路径
config_path = "/root/workspace/dev_ph/script/config"

-- 所有的配置文件(没有包含就不会读取)
config_file_path =
{
	"day_login.lua",
	"time_slot_login.lua",
	"item.lua",
	"province.lua",
}

-- 中间件文件路径
temp_config_path = "/root/workspace/test/act_manager/temp_config"

-- 监听的进程id文件产生的路径
listening_process_path = "/root/workspace/dev_ph/script/config"

-- 可监听的进程id文件的名字
listening_process_config =
{
	[1] = {"gamesvr.pid"},
	[2] = {"center.pid"},
	[3] = {"chatsvr.pid"},
	[4] = {"datasvr.pid"},
	[5] = {"gamesvr.pid"},
	[6] = {"jackpot.pid"},
	[7] = {"logger.pid"},
	[8] = {"logon.pid"},
	[9] = {"mailsvr.pid"},
	[10] = {"roomcenter.pid"},
	[11] = {"roomsvr.pid"},
	[12] = {"teamsvr.pid"},
	[13] = {"usercenter.pid"},
}

-- 使用脚本路径
script_operate_path = "/root/workspace/dev_ph"
-- 使用脚本
third_party_config = 
{
	{"重刷脚本", "reload_all.sh"},
	{"关闭服务器", "stop_all.sh"},
	{"开启服务器", "start.sh"},
}
