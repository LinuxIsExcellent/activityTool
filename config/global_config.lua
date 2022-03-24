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
	--"time_slot_login.lua",
	--"item.lua",
	--"province.lua",
	--"bonus_rain.lua",
	--"task.lua",
	--"item_group.lua",
    --"coin_rooms.lua",
}

-- 键值对一维展开的配置文件
config_list_file_path =
{
	"pay_activity_config.lua",
}

-- 中间件文件路径,根据在客户端操作表格自动生成的表的信息（字段顺序，字段标注，活动的字段组合...）
temp_config_path = "/root/workspace/activityTool/table_info"

-- 监听的进程id文件产生的路径
listening_process_path = "/root/workspace/dev_ph"
-- 可监听的进程id文件的名字
listening_process_config =
{
	[1] = {"gamesvr.pid", "asda"},
	[2] = {"center.pid", "asdas"},
	[3] = {"chatsvr.pid", "聊天服"},
	[4] = {"datasvr.pid", "数据服"},
	[5] = {"jackpot.pid", "jackpot服"},
	[6] = {"logger.pid", "日志服"},
	[7] = {"logon.pid", "登录服"},
	[8] = {"mailsvr.pid", "邮件服"},
	[9] = {"roomcenter.pid", "房间中心服"},
	[10] = {"roomsvr.pid", "房间服"},
	[11] = {"teamsvr.pid", "战队服"},
	[12] = {"usercenter.pid", "用户中心服"},
}

-- 使用脚本路径
script_operate_path = "/root/workspace/activityTool/shell_script"
-- 使用脚本
shell_config = 
{
	["关闭服务器"] = "stop_server.sh",
	["开启服务器"] = "start_server.sh",
	["重刷脚本"] = "reload_server.sh",
}
