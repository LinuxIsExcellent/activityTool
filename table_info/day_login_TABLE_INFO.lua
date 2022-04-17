day_login_TABLE_INFO =
{
	["###cell_color###"] =
	{
	},
	["###col_width###"] =
	{
		-- 1
		{
			field_name = "100",
		},
		-- 2
		{
			field_name = "100",
		},
		-- 3
		{
			field_name = "100",
		},
		-- 4
		{
			field_name = "274",
		},
		-- 5
		{
			field_name = "100",
		},
		-- 6
		{
			field_name = "100",
		},
		-- 7
		{
			field_name = "100",
		},
		-- 8
		{
			field_name = "100",
		},
		-- 9
		{
			field_name = "100",
		},
		-- 10
		{
			field_name = "100",
		},
		-- 11
		{
			field_name = "100",
		},
	},
	["###field_sequence###"] =
	{
		-- 1
		{
			field_name = "id",
		},
		-- 2
		{
			field_name = "start_time",
		},
		-- 3
		{
			field_name = "end_time",
		},
		-- 4
		{
			field_name = "week_num_list",
		},
		-- 5
		{
			field_name = "reg_day_low",
			field_link = "table#item_group#id",
		},
		-- 6
		{
			field_name = "reg_day_high",
		},
		-- 7
		{
			field_name = "group",
		},
		-- 8
		{
			field_name = "channel",
		},
		-- 9
		{
			field_name = "ban_channel",
		},
		-- 10
		{
			field_name = "item_group",
			field_link = "table#item_group#id",
		},
	},
	["###row_height###"] =
	{
		-- 1
		{
			field_name = "30",
		},
		-- 2
		{
			field_name = "30",
		},
		-- 3
		{
			field_name = "30",
		},
	},
	["field_sequence"] =
	{
		-- 1
		{
			field_name = "id",
			field_desc = "一个唯一的id",
		},
		-- 2
		{
			field_name = "reg_day_low",
			field_desc = "注册时间大于多少天的用户",
			field_link = "table#item#id",
		},
		-- 3
		{
			field_name = "end_time",
			field_desc = "结束时间阿松大 撒 阿萨",
		},
		-- 4
		{
			field_name = "item_group",
			field_desc = "奖励组",
			field_link = "table#day_login#group",
		},
		-- 5
		{
			field_name = "group",
			field_desc = "包体",
		},
		-- 6
		{
			field_name = "week_num_list",
			field_desc = "每周的开放天数",
		},
		-- 7
		{
			field_name = "channel",
			field_desc = "奖励组啊啊啊",
		},
		-- 8
		{
			field_name = "start_time",
			field_desc = "开始时间",
		},
		-- 9
		{
			field_name = "reg_day_high",
			field_desc = "注册时间小于多少天的用户",
		},
		-- 10
		{
			field_name = "ban_channel",
			field_desc = "禁止渠道",
		},
	},
	["field_sequence_3"] =
	{
	},
	["field_sequence_3_3"] =
	{
	},
	["group%ARRAY"] =
	{
		-- 1
		{
			field_name = "name",
		},
		-- 2
		{
			field_name = "age",
		},
	},
	["table1"] =
	{
		-- 1
		{
			field_name = "end_time",
		},
		-- 2
		{
			field_name = "begin_time",
		},
		-- 3
		{
			field_name = "buy_count",
		},
		-- 4
		{
			field_name = "consumes",
		},
		-- 5
		{
			field_name = "gold_config",
		},
	},
	["table1#consumes"] =
	{
		-- 1
		{
			field_name = "item_count",
		},
		-- 2
		{
			field_name = "item_id",
		},
	},
	["table1#gold_config%ARRAY"] =
	{
		-- 1
		{
			field_name = "tips_en",
		},
		-- 2
		{
			field_name = "prob",
		},
		-- 3
		{
			field_name = "count",
		},
		-- 4
		{
			field_name = "tips_th",
		},
	},
	["week_num_list"] =
	{
		-- 1
		{
			field_name = "item_id",
		},
		-- 2
		{
			field_name = "count",
		},
		-- 3
		{
			field_name = "item",
		},
	},
}
