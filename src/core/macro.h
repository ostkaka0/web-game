// Copyright (c) 2016 John Emanuelsson - All Rights Reserved

#pragma once

template <typename F>
struct _ScopeExit {
	_ScopeExit(F f) : f(f) {}
	~_ScopeExit() { f(); }
	F f;
};

template <typename F>
_ScopeExit<F> _MakeScopeExit(F f) {
	return _ScopeExit<F>(f);
};

#define STRING_JOIN2(arg1, arg2) DO_STRING_JOIN2(arg1, arg2)
#define DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2
#define SCOPE_EXIT(code) auto STRING_JOIN2(scope_exit_, __LINE__) = _MakeScopeExit([&](){code;})
#define SCOPE(begin, end) begin; SCOPE_EXIT(end)
#define SCOPE_OBJ(obj, begin, end) (obj).begin; SCOPE_EXIT((obj).end)
 
