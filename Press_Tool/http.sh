# -seconds 压测时长（s）
nohup ./http_load -timeout 10 -parallel 50 -seconds 600  http.log >result.test 2>&1 &
nohup ./http_load -timeout 10 -parallel 50 -seconds 600  http.log.2 > result.base 2>&1 &
# result
# fetches/sec       qps
# msecs/connect     平响

