#pragma once

class AbstractConnection
{
public:
	virtual ~AbstractConnection() {}
	virtual void connect() = 0;
};
