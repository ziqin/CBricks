#!/usr/bin/env python3
import networkx as nx
from collections import deque


def subset_construction(nfa, start, alphabet):
    def state_epsilon_closure(s, know_closure=None):
        if s in nfa.nodes[s]:
            return nfa.nodes[s]['closure']
        closure = know_closure if know_closure else set()
        closure.update([s])
        for neighbor, attr in nfa[s].items():
            if neighbor not in closure and attr['e'] == '':
                closure.update(state_epsilon_closure(neighbor, closure))
        if know_closure is None:
            nfa.nodes[s]['closure'] = closure
        return closure

    def states_epsilon_closure(s):
        s = tuple(s)
        if s in nfa.graph:
            return nfa.graph[s]
        closure = set()
        for e in s:
            closure.update(state_epsilon_closure(e))
        nfa.graph[s] = closure
        return closure

    def move(s, c):
        reachable = set()
        for e in s:
            for neighbor, attr in nfa[e].items():
                if attr['e'] == c:
                    reachable.add(neighbor)
        return reachable

    mark = set()
    d_trans = {}
    states_queue = deque([tuple(state_epsilon_closure(start))])
    while len(states_queue) > 0:
        t = states_queue.popleft()
        if t in mark:
            continue
        mark.add(t)
        for c in alphabet:
            u = tuple(states_epsilon_closure(move(t, c)))
            if u not in mark:
                states_queue.append(u)
            if t in d_trans:
                d_trans[t][c] = u
            else:
                d_trans[t] = {c: u}
    return d_trans


def main():
    fsa = nx.DiGraph()
    fsa.add_edge(0, 1, e='')
    fsa.add_edge(0, 3, e='')
    fsa.add_edge(1, 2, e='a')
    fsa.add_edge(3, 4, e='b')
    fsa.add_edge(2, 5, e='')
    fsa.add_edge(4, 5, e='')
    fsa.add_edge(5, 0, e='')
    fsa.add_edge(0, 5, e='')
    fsa.add_edge(5, 6, e='a')
    fsa.add_edge(6, 7, e='')
    fsa.add_edge(6, 9, e='')
    fsa.add_edge(7, 8, e='a')
    fsa.add_edge(9, 10, e='b')
    fsa.add_edge(8, 11, e='')
    fsa.add_edge(10, 11, e='')
    fsa.add_edge(11, 12, e='')
    fsa.add_edge(11, 14, e='')
    fsa.add_edge(12, 13, e='a')
    fsa.add_edge(14, 15, e='b')
    fsa.add_edge(13, 16, e='')
    fsa.add_edge(15, 16, e='')
    start = 0
    ac = 16
    dfa = subset_construction(fsa, start, ['a', 'b'])
    mapping = {}
    base = ord('A')
    for key in dfa.keys():
        mapping[key] = chr(base)
        base += 1
    for state, trans in dfa.items():
        print(mapping[state], {key: mapping[val] for key, val in trans.items()}, '.' if ac in state else ' ', state)


if __name__ == '__main__':
    main()
