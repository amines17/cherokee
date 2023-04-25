base_address=localhost:8080

echo "TEST FONCTIONNEL DU PROJET CHEROKEE"
echo 

test_http_get_head() {

    echo "Test $4 : $1"

    expect_code=$2
    expect_body=$3
    method=$4

    statuscode=$(curl -X $4 --silent -w "%{http_code}" \
        -o >(cat >/tmp/curl_body) \
        "$1"
    ) 
    body="$(cat /tmp/curl_body)"

    if [ "$statuscode" -eq $expect_code ] 
    then 
        echo "Test $statuscode code pass" 
    else 
        echo "Test $statuscode status fail" 
    fi

    if [ "$4" = "GET" ]
    then
        if [ "$body" = "$(cat $expect_body)" ] 
        then 
            echo "Test body code pass" 
        else 
            echo "Test body fail" 
        fi
    else #head 
        if [ -z "$body" ] 
        then 
            echo "Test body code pass" 
        else 
            echo "Test body fail" 
        fi
    fi 

    echo 
}

test_http_post_put() {

    echo "Test $5 : $1"

    expect_code=$2
    expect_body=$3
    data=$4 

    statuscode=$(curl -X $5 -d "$data" --silent -w "%{http_code}" \
        -o >(cat >/tmp/curl_body) \
        "$1"
    ) 
    
    body="$(cat /tmp/curl_body)"

    if [ "$statuscode" -eq $expect_code ] 
    then 
        echo "Test $statuscode status code pass" 
    else 
        echo "Test $statuscode status fail" 
    fi

    if [ "$body" = "$(cat $expect_body)" ] 
    then 
        echo "Test body code pass" 
    else 
        echo "Test body fail" 
    fi 

    echo 
}

test_http_delete() {

    echo "Test DELETE : $1"

    expect_code=$2
    expect_body=$3

    statuscode=$(curl -X DELETE --silent -w "%{http_code}" \
        -o >(cat >/tmp/curl_body) \
        "$1"
    ) 
    body="$(cat /tmp/curl_body)"

    if [ "$statuscode" -eq $expect_code ] 
    then 
        echo "Test $statuscode status code pass" 
    else 
        echo "Test $statuscode status fail" 
    fi

    if [ -f "$expect_body" ] 
    then 
        echo "test delete fail" 
    else 
        echo "Test delete pass" 
    fi 

    echo 
}



test_http_get_head $base_address/index.html 200 index.html GET
test_http_get_head $base_address/form.html 200 form.html GET
test_http_get_head $base_address/noexist.html 404 http_status_code/404.html GET
test_http_post_put $base_address/thisisatest 201 http_status_code/201.html "THIS IS A TEST" POST
test_http_get_head $base_address/thisisatest 200 thisisatest GET
test_http_post_put $base_address/thisisatest 201 http_status_code/201.html "THIS IS A TEST" PUT
test_http_get_head $base_address/thisisatest 200 thisisatest HEAD
test_http_delete $base_address/thisisatest 204 thisisatest

echo "FIN DES TEST FONCTIONNEL"
echo
