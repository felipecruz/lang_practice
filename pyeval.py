ops = {
    '+': lambda a, b: a + b,
    '-': lambda a, b: a - b,
    '/': lambda a, b: a / b,
    '*': lambda a, b: a * b
}

def rev_polish_eval(content):
    tokenized = content.split(' ')
    return _reverse_polish_eval(tokenized)

def polish_eval(content):
    tokenized = content.split(' ')
    return _polish_eval(tokenized)[0]

def _reverse_polish_eval(tokens):
    stack = []
    for token in tokens:
        if token in ops:
            a1 = stack.pop()
            a2 = stack.pop()
            stack.append(ops[token](a1, a2))
        else:
            stack.append(int(token))

    return stack.pop()

def _polish_eval(tokens):
    if not tokens:
        return (None, 0)

    if len(tokens) == 2:
        return (int(tokens[1]), 1)

    token = tokens.pop(0)
    if not token in ops:
        return (int(token), 0)
    elif tokens[0] not in ops and tokens[1] not in ops:
        result = ops[token](int(tokens[0]), int(tokens[1]))
        tokens.pop(0)
        tokens.pop(0)
        return (result, 2)
    else:
        t1 = tokens[0:]
        t2 = tokens[1:]
        expr1, removed = _polish_eval(t1)
        for i in range(removed):
            t2.pop(0)
        expr2, removed = _polish_eval(t2)
        for i in range(removed):
            t1.pop(0)
        result = ops[token](expr1, expr2)
        return (result, 0)


def test_reverse_polish_eval():
    assert 1 == rev_polish_eval("1")
    assert 2 == rev_polish_eval("2")
    assert 2 == rev_polish_eval("1 1 +")
    assert 3 == rev_polish_eval("1 2 +")
    assert 9 == rev_polish_eval("3 3 + 3 +")
    assert 9 == rev_polish_eval("3 3 *")
    assert -2 == rev_polish_eval("7 2 3 + -")
    assert 3 == rev_polish_eval("1 1 1 + +")


def test_polish_eval():
    assert 1 == polish_eval("1")
    assert 2 == polish_eval("2")
    assert 2 == polish_eval("+ 1 1")
    assert 4 == polish_eval("+ 1 3")
    assert 6 == polish_eval("+ + 1 2 3")
    assert 7 == polish_eval("+ 1 * 2 3")
    assert 12 == polish_eval("+ + + 1 2 3 6")
