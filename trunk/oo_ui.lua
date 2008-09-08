print("Hello, LUA!")

set_count(6)
set_icon(48,48,"BMP",65535)
set_icon(144,48,"CAM",65535)
set_icon(240,48,"CCTV",65535)
--set_icon(48,144,"4",65535)
--set_icon(144,144,"5",65535)
--set_icon(240,144,"6",65535)

set_cmd("/root/lua/oo_bmp", "oo_bmp");
set_cmd("/root/cam_view", "cam_veiw");
set_cmd("/root/cctv_client", "cctv");
