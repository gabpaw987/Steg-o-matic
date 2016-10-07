#include "provided.h"
#include "http.h"
#include <string>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
	string page;
	if (!HTTP().get(url, page))
		return false;

	return Steg::hide(page, msg, host);
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	string page;
	if (!HTTP().get(url, page))
		return false;

	return Steg::reveal(page, msg);
}
