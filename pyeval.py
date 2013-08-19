ops = {
    '+': lambda a, b: a + b,
    '-': lambda a, b: a - b,
    '/': lambda a, b: a / b,
    '*': lambda a, b: a * b
}

def rev_polish_eval(content):
    tokenized = content.split(' ')
    return _reverse_polish_eval(tokenized)

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

def test_reverse_polish_eval():
    assert 1 == rev_polish_eval("1")
    assert 2 == rev_polish_eval("2")
    assert 2 == rev_polish_eval("1 1 +")
    assert 3 == rev_polish_eval("1 2 +")
    assert 9 == rev_polish_eval("3 3 + 3 +")
    assert 9 == rev_polish_eval("3 3 *")
    assert -2 == rev_polish_eval("7 2 3 + -")
    assert 3 == rev_polish_eval("1 1 1 + +")
