curl --resolve default_server:8081:0.0.0.0 http://default_server:8081
curl --resolve virtual_server:8081:0.0.0.0 http://virtual_server:8081
curl -i -F upfile=@/tmp/index.html localhost:8080/upload/
curl -i -X GET -F upfile=@/tmp/index.html localhost:8080/upload/
curl -i -X GET localhost:8080/upload
curl -i -X POST -F upfile=@./conf/default.conf localhost:8080/upload/
curl -i -X POST -F upfile=@./conf/default.conf localhost:8080/upload/ -H"Transfer-Encoding: chunked"
curl -i -X POST -F upfile=@/tmp/index.html localhost:8080/upload/
curl -i -X POST localhost:8080
curl -i http://localhost:8081/ -H"Host: default_server"
curl -i http://localhost:8081/ -H"Host: virtual_server"
curl -i localhost:8080
curl -i localhost:8081
curl -i localhost:8080 -H"Host: test,test,test"
curl -i localhost:8080 -H"Host:"
curl -i localhost:8080 -H"Host:"
curl -i localhost:8080 -H"Host:''"
curl -i localhost:8080 -H"Transfer-Encoding: test"
curl -i localhost:8080 -X POST
curl -i localhost:8080 -X PUT
curl -i localhost:8080/aaa
curl -i localhost:8080/aaa -X PUT
curl -i localhost:8080/aaaa
curl -i localhost:8080/aaaa -X POST
curl -i localhost:8080/aaaa -X UNKNOWN
curl -i localhost:8080/cgi/
curl -i localhost:8080/cgi/cgi
curl -i localhost:8080/cgi/cgi.sh
curl -i localhost:8080/cgi/test.py
curl -i localhost:8080/index/
curl -i localhost:8080/index/cgi/
curl -i localhost:8080/index/cgi/cgi.sh
curl -i localhost:8080/index/error_pages
curl -i localhost:8080/index/error_pages/
curl -i localhost:8080/index/upload/
curl -i localhost:8080/upload/
curl -i localhost:8080/upload/ -X ""
curl -i localhost:8080/upload/ -X DELETE
curl -i localhost:8080/upload/ -X POST
curl -v -XPOST http://localhost:8080/a.txt -H"Host: localhost2" -d01234567891
curl -v -XPOST http://localhost:8080/upload/ -H"Host: localhost" -d01234567891
curl -v -XPOST http://localhost:8080/upload/ -H"Host: localhost2" -d01234567891
curl -v localhost:8080/delete/167534789764417.txt
curl -v localhost:8080/delete/167534789764417.txt -X DELETE
curl -v localhost:8080/delete/167534789764417.txt -X POST
curl -v localhost:8081 -H"virtual_server"
