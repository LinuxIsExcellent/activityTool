-- 服务器ip
server_ip = "localhost"
-- 服务器端口
server_port = 23543

-- 配置文件路径
config_path = "/root/workspace/dev_ph/script/config"

-- 二维表展开的配置文件
config_file_path =
{
	"day_login.lua",
	-- "time_slot_login.lua",
	-- "item.lua",
	-- "province.lua",
	-- "bonus_rain.lua",
	-- "task.lua",
	-- "item_group.lua",
    -- "coin_rooms.lua",
}

-- 键值对一维展开的配置文件
config_list_file_path =
{
	-- "global_activity.lua",
}

-- 中间件文件路径,根据在客户端操作表格自动生成的表的信息（字段顺序，字段标注，活动的字段组合...）
temp_config_path = "/root/workspace/activityTool/table_info"

-- 监听的进程id文件产生的路径
listening_process_path = "/root/workspace/dev_ph/script/config"

-- 可监听的进程id文件的名字
listening_process_config =
{
	[1] = {"gamesvr.pid", "服务1"},
	[2] = {"center.pid", "服务2"},
	[3] = {"chatsvr.pid", "服务3"},
	[4] = {"datasvr.pid", "服务4"},
	[5] = {"jackpot.pid", "服务5"},
	[6] = {"logger.pid", "服务6"},
	[7] = {"logon.pid", "服务7"},
	[8] = {"mailsvr.pid", "服务8"},
	[9] = {"roomcenter.pid", "服务9"},
	[10] = {"roomsvr.pid", "服务10"},
	[11] = {"teamsvr.pid", "服务11"},
	[12] = {"usercenter.pid", "服务12"},
}

-- 使用脚本路径
script_operate_path = "/root/workspace/activityTool/shell_script"
-- 使用脚本
shell_config = 
{
	-- ["重刷脚本"] = "reload_all.sh",
	["关闭服务器"] = "stop_server.sh",
	["开启服务器"] = "start_server.sh",
}
