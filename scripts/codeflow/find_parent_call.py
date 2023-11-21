from sys import argv

def compare_arrays(v1, v2):
    if len(v1) != len(v2):
        return False
    for i in range(len(v1)):
        if v1[i] != v2[i]:
            return False
    return True

if len(argv) != 2:
    print("Expected symbol as argv[1]")
    exit()

symbol = argv[1].strip()
calls = []
func_index = []
parent_tree = []
with open("output.txt", "r") as file:
    i = 0
    for line in file:
        line = line[:-3]
        func_symbol = line.strip()
        calls.append((len(line) - len(func_symbol), func_symbol))
        if func_symbol == symbol:
            func_index.append(i)
        i += 1

for i in func_index:
    curr_space = calls[i][0]
    call_tree = []
    for j in range(i - 1, -1, -1):
        space, func_name = calls[j]
        if space < curr_space:
            curr_space = space
            call_tree.append(func_name)
    if len(parent_tree) == 0:
        parent_tree.append(call_tree)
    else:
        should_add = True
        for arr in parent_tree:
            if compare_arrays(arr, call_tree):
                should_add = False
                break
        if should_add:
            parent_tree.append(call_tree)

k = -1
loop = len(parent_tree) > 1
while loop:
    element = parent_tree[0][k]
    for arr in parent_tree:
        if arr[k] != element:
            loop = False
            break
    if not loop:
        for i in range(len(parent_tree)):
            parent_tree[i] = parent_tree[i][:k + 1]
        break
    k -= 1

for arr in parent_tree:
    print(" -> ".join(list(reversed(arr))) + " -> " + symbol + "\n")