print("Hello, LUA!")

SET_COUNT(11)

SET_BGIMAGE("/root/bmp/gomez_bg.bmp")
SET_BGCOLOR(0,0,0,255,255,255)
--SET_ICON(40,40,1,"/root/bmp/bmpIcon1.bmp",65535)
SET_ICON(125,40,1,"/root/bmp/camIcon1.bmp",65535)
SET_ICON(210,40,1,"/root/bmp/cctvIcon1.bmp",65535)
SET_ICON(40,90,1,"/root/bmp/drawClient.bmp",65535)
SET_ICON(125,90,1,"/root/bmp/drawservIcon1.bmp",65535)
SET_ICON(210,90,1,"/root/bmp/circleIcon1.bmp",65535)
SET_ICON(40,140,1,"/root/bmp/luaIcon1.bmp",65535)
SET_ICON(125,140,1,"/root/bmp/alcaIcon1.bmp",65535)
SET_ICON(210,140,1,"/root/bmp/alcaservIcon1.bmp",65535)
--SET_ICON(40, 190,1,"/root/bmp/tiltclnt.bmp",65535)
--SET_ICON(125, 190,1,"/root/bmp/tiltserv.bmp",65535)


--set_icon(48,144,"4",65535)
--set_icon(144,144,"5",65535)
--set_icon(240,144,"6",65535)

--SET_CMD("/root/oo_bmp_viewer", "oo_bmp_viewer");
SET_CMD("/root/cam_view", "cam_view");
SET_CMD("/root/cctv_client", "cctv");
SET_CMD("/root/drawtogether_clnt", "drawtogether_clnt");
SET_CMD("/root/drawtogether_serv", "drawtogether_serv");
SET_CMD("/root/MoveCircle_Client", "MoveCircle_Client");
SET_CMD("/root/oo_lua", "oo_lua");
SET_CMD("/root/AlcaGame_Network_Client_1", "AlcaGame_Network_Client_1");
SET_CMD("/root/AlcaGame_Sensor_Client", "AlcaGame_Sensor_Client");
--SET_CMD("/root/tilt_clnt", "tilt_clnt");
--SET_CMD("/root/tilt_serv", "tilt_serv");

