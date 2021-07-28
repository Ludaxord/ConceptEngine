#pragma once

class CEException : public std::exception {
public:
	char const* what() const override = 0;
};

class CEEngineBootException: public CEException {
public:
	char const* what() const override {
		return "Failed to resolve Engine Boot, Engine not loaded...";
	};
};
