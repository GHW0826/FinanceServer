#pragma once

class DBConfig
{
public:
	DBConfig(const String& mode, const String& connectionString)
		: _mode(mode)
		, _connectionString(connectionString) 
	{
	}
private:
	String _mode;
	String _connectionString;
};

