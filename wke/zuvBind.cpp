//////////////////////////////////////////////////////////////////////////


#include <WebCore/config.h>
#include <JavaScriptCore/JSGlobalObject.h>
#include <JavaScriptCore/JSFunction.h>
#include <JavaScriptCore/SourceCode.h>
#include <JavaScriptCore/JavaScript.h>
#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/Completion.h>
#include <JavaScriptCore/OpaqueJSString.h>
#include <WebCore/GCController.h>
#include <WebCore/JSDOMWindowCustom.h>
#include <WebCore/Page.h>
#include <WebCore/Frame.h>
#include <WebCore/Chrome.h>
#include <WebCore/ChromeClient.h>

#include "wkeDebug.h"

//zero 需要绑定的头文件

#include <SQLite\sqlite3.h>
#include <WinSock2.h>
#include <curl\curl.h>


#include "duv_module.h"

//cexer: 必须包含在后面，因为其中的 wke.h -> windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wkeWebView.h"


ZuvFuncs func;
//////////////////////////////////////////////////////////////////////////

//sqlite

zuv_ret_t duv_sqlite_open(zuv_context ctx){
	int a = func.zuv_get_top(ctx);
	if (func.zuv_is_string(ctx, 0)){
		const char *filename = func.zuv_get_string(ctx, 0);
		sqlite3 *db = NULL;
		if (sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL) != SQLITE_OK){
			//数据库不存在则创建
			if (sqlite3_open_v2(filename, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK){
				//加密数据库
				if (func.zuv_is_string(ctx, 1)){
					int len = 0;
					const char * key = func.zuv_get_lstring(ctx, 1, &len);
					if (sqlite3_rekey(db, key, len) != SQLITE_OK){
						sqlite3_close(db);
						func.zuv_push_null(ctx);
						return 0;
					}
				}
				func.zuv_push_pointer(ctx, db);
				return 1;
			}
			//打开并创建失败
			func.zuv_push_null(ctx);
		}
		else
		{
			//加密数据库
			if (func.zuv_is_string(ctx, 1)){
				int len = 0;
				const char * key = func.zuv_get_lstring(ctx, 1, &len);
				if (sqlite3_key(db, key, len) != SQLITE_OK){
					sqlite3_close(db);
					func.zuv_push_null(ctx);
					return 0;
				}
			}
			func.zuv_push_pointer(ctx, db);
		}
		return 1;
	}
	return 0;
}

static zuv_ret_t duv_sqlite_exec(zuv_context ctx){
	if (!func.zuv_is_pointer(ctx, 0) || !func.zuv_is_string(ctx, 1))
		return 0;
	sqlite3 *db = (sqlite3 *)func.zuv_get_pointer(ctx, 0);
	const char*sql = func.zuv_get_string(ctx, 1);

	sqlite3_stmt *stmt;//结果集

	int ret, type;;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	//获取列数目
	int n_columns = sqlite3_column_count(stmt);

	int j = 0;
	zuv_idx_t arr_idx = func.zuv_push_array(ctx);	//创建一个数组用来存放返回值
	do{
		ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW)
		{
			//处理每一列
			zuv_idx_t obj_idx = func.zuv_push_object(ctx);//创建列对象
			for (int i = 0; i < n_columns; i++)
			{
				/*获取列存储类型*/
				type = sqlite3_column_type(stmt, i);
				switch (type)
				{
				case SQLITE_INTEGER:
					/*处理整型*/
					func.zuv_push_int(ctx, sqlite3_column_int(stmt, i));//字段值
					break;
				case SQLITE_FLOAT:
					/*处理浮点数*/
					func.zuv_push_number(ctx, sqlite3_column_double(stmt, i));//字段值
					break;
				case SQLITE_TEXT:
					/*处理字符串*/
					func.zuv_push_string(ctx, (const char *)sqlite3_column_text(stmt, i));//字段值
					break;
				case SQLITE_BLOB:{
					/*处理二进制*/
					int len = sqlite3_column_bytes(stmt, i);
					const void *blob = sqlite3_column_blob(stmt, i);
					void *out = func.zuv_push_buffer_raw(ctx, len, 0);
					memcpy(out, blob, len);
					break;
				}
				case SQLITE_NULL:
					/*处理空*/
					func.zuv_push_null(ctx);
				}
				func.zuv_put_prop_string(ctx, obj_idx, sqlite3_column_name(stmt, i));//字段名
			}
			func.zuv_put_prop_index(ctx, arr_idx, j);//设置数组索引
			j++;
		}
		else //if (ret == SQLITE_DONE) //结束
		{
			break;
		}
	} while (true);
	return 1;
}
static zuv_ret_t duv_sqlite_close(zuv_context ctx){
	if (!func.zuv_is_pointer(ctx, 0))
		return 0;
	sqlite3 *db = (sqlite3 *)func.zuv_get_pointer(ctx, 0);
	if (sqlite3_close(db) != SQLITE_OK){
		func.zuv_push_false(ctx);
	}
	else
	{
		func.zuv_push_true(ctx);
	}
	return 1;
}

static const zuv_function_list_entry zuv_sqlite_funcs[] = {
		{ "open", duv_sqlite_open, 2 },
		{ "exec", duv_sqlite_exec, 3 },
		{ "close", duv_sqlite_close, 1 },
		{ NULL, NULL, 0 },
};

static zuv_ret_t zuvopen_slite(zuv_context ctx) {

	func.zuv_push_global_object(ctx);
	func.zuv_put_function_list(ctx, -1, zuv_sqlite_funcs);
	func.zuv_put_prop_string(ctx, -1, "sqlite");
	return 0;
}


//curl

static const zuv_function_list_entry zuv_curl_funcs[] = {
		{ "open", duv_sqlite_open, 1 },
		{ "exec", duv_sqlite_close, 1 },
		{ "close", duv_sqlite_close, 1 },
		{ NULL, NULL, 0 },
};

static zuv_ret_t zuvopen_curl(zuv_context ctx) {

	func.zuv_push_global_object(ctx);
	func.zuv_put_function_list(ctx, -1, zuv_curl_funcs);
	func.zuv_put_prop_string(ctx, -1, "curl");
	return 0;
}

WKE_API void       WKE_CALL  ZuvModuleInit(ZuvFuncs *f){
	memcpy(&func, f, sizeof(ZuvFuncs));
	func.zuv_add_module(zuvopen_slite);
	//func.zuv_add_module(zuvopen_curl);
}
