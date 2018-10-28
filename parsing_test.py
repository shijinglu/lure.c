from subprocess import Popen, PIPE, STDOUT


def parse(s):
    """
    invoke `echo $s | ./lure` and collect output and error
    """
    p = Popen(["./lure"], stdout=PIPE, stdin=PIPE, stderr=PIPE)
    return p.communicate(input=s)

def test_debug():
    cases = [
        "USER_TAGS IN ['admin', 'tester'] ",
        "CITY_ID IN [1, 2, 3]",
        "3.2.1 <= VERSION < 3.4.5",
        "DEVICE_OS == ios AND RISK_SCORE > 0.85",
        "P_VALUE < 0.05",
        "10 < md5mod(USER_ID, 100) <= 20",
    ]
    for s in cases:
        out, err = parse(s)
        print('[0]----> input: {} <-----'.format(s))
        print('[1]----> output: {} <-----'.format(out))
        print('[2]----> error: {} <-----'.format(err))

    assert False

# def test_simple():
#     s = 'CITY_ID > 10'
#     out, err = parse(s)
#     print('----> out = {}, err = {} <-----'.format(out, err))
#     assert not out

