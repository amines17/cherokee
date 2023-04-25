#include <criterion/criterion.h>
#include "../src/headers/http_header_parser.h"
#include "../src/lib/http_header_parser.c"


Test(get_header_with_data, header_with_data) {
    const char *data_client = "GET / HTTP/1.1\r\n"
                              "Host: www.test.com\r\n"
                              " Content-Type: application/json\r\n"
                              "Content-Length: 0\r\n"
                              "\r\n";

    size_t data_size = strlen(data_client);

    http_parse_header_t *header = get_header_with_data(data_client, data_size);

    cr_assert_not_null(header);
    cr_assert_str_eq(header->lien, "/");
    cr_assert_str_eq(header->method, "GET");
    cr_assert_str_eq(header->http_version, "HTTP/1.1");
    cr_assert_str_eq(header->content_type, " application/json");

    destroy_parse_header(&header);
}

Test(get_header_with_data, header_with_data_put)
{
    const char* data = "PUT / HTTP/1.1\r\n"
                       "Host: 127.0.0.1:9500\r\n"
                       "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:108.0) Gecko/20100101 Firefox/108.0\r\n"
                       "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
                       "Accept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\n"
                       "Connection: keep-alive\r\n"
                       "Upgrade-Insecure-Requests: 1\r\n"
                       "Sec-Fetch-Dest: document\r\n"
                       "Sec-Fetch-Mode: navigate\r\n"
                       "Sec-Fetch-Site: none\r\n"
                       "Sec-Fetch-User: ?1\r\n"
                       "\r\n"
                       "title=123&image=456%40aaa.com&subtitle=chien";


    http_parse_header_t* result = get_header_with_data(data, strlen(data));

    cr_assert_not_null(result);

    destroy_parse_header(&result);
}

Test(parse_line_header, valid_header) {
    http_parse_header_t header = {0};

    parse_line_header(&header, "Content-Length: 123\r\n");

    cr_assert_str_eq(header.content_length, " 123");
    cr_assert_null(header.content_type);
}

Test(parse_line_header, invalid_header) {
    http_parse_header_t header = {0};

    parse_line_header(&header, "X-Unknown-Header: value\r\n");

    cr_assert_null(header.content_type);
    cr_assert_null(header.content_length);
}

Test(insert_body, test_success_valid_body)
{
    const char *body_data = "This is the body data.";
    http_parse_header_t header = {0};
    insert_body(&header, body_data, strlen(body_data));

    cr_assert_str_eq(header.body, body_data);
}

Test(insert_body, test_fail_without_header)
{
    const char *body_data = "This is the body data.";
    insert_body(NULL, body_data, strlen(body_data));

    cr_assert(true);
}

Test(destroy_parse_header, test_fail_without_header) {
    http_parse_header_t* header = NULL;
    destroy_parse_header(&header);
    cr_assert_null(header);
}

Test(create_struct_parse, create_struct_parse)
{
    http_parse_header_t *header = create_struct_parse();
    char *lien = "http://test.com";
    char *method = "GET";
    char *http_version = "HTTP/1.1";
    char *content_type = "text/plain";
    char *content_length = "100";
    char *body = "This is the body";

    header->lien = lien;
    header->method = method;
    header->http_version = http_version;
    header->content_type = content_type;
    header->content_length = content_length;
    header->body = body;

    cr_assert_not_null(header);
    cr_assert_str_eq(header->lien, lien);
    cr_assert_str_eq(header->method, method);
    cr_assert_str_eq(header->http_version, http_version);
    cr_assert_str_eq(header->content_type, content_type);
    cr_assert_str_eq(header->content_length, content_length);
    cr_assert_str_eq(header->body, body);

    free(header);
}

Test(create_struct_parse,  invalid_create_struct_parse)
{
    http_parse_header_t *header = create_struct_parse();
    
    cr_assert_not_null(header);
    cr_assert_null(header->lien);
    cr_assert_null(header->method);
    cr_assert_null(header->http_version);
    cr_assert_null(header->content_type);
    cr_assert_null(header->content_length);
    cr_assert_null(header->body);
    
    free(header);
}
