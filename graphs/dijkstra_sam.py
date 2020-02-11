import sys, heapq

class GraphNode(object):
    def __init__(self, name):
        self.name = name
        self.adj = []

        self.visited = False

    def add_neighbour(self, neigh, weight):
        self.adj.append((neigh, weight))

    def __lt__(self, other):
        return id(self) < id(other)

class PriorityQueue(object):
    def __init__(self):
        self.q = []

    def append(self, item, weight):
        heapq.heappush(self.q, (weight, item))

    def pop(self):
        return heapq.heappop(self.q)

class Scenario(object):
    def __init__(self):
        self.node_lookup = dict()

    def _read_low(self, pred):
        while True:
            l = sys.stdin.readline()
            if not l:
                raise EOFError()
            try:
                return pred(l.strip())
            except:
                continue

    def _read_int(self):
        return self._read_low(lambda x: int(x))

    def _read_string(self):
        def pred(x):
            if len(x) == 0: raise Exception()
            return x

        return self._read_low(pred)

    def _read_link(self):
        def pred(x):
            x = x.split(maxsplit=2)
            if len(x[0]) == 0 or len(x[1]) == 0:
                raise Exception()
            return [x[0], x[1], int(x[2])]

        return self._read_low(pred)

    def _read_testcase(self):
        def pred(x):
            if len(x) == 0:
                raise Exception()
            x = [i.strip() for i in x.split(maxsplit=1)]
            for i in x:
                if len(i) == 0:
                    raise Exception()
            return x

        return self._read_low(pred)

    def _add_place(self, index, name):
        self.node_lookup[name] = GraphNode(name)

    def _add_link(self, name1, name2, weight):
        n1 = self.node_lookup[name1]
        n2 = self.node_lookup[name2]

        n1.add_neighbour(n2, weight)
        n2.add_neighbour(n1, weight)

    def _dijkstra(self, start, end):
        pending = PriorityQueue()
        visited = []
        so_far = 0

        def cleanup():
            for i in visited:
                i.visited = False

        while True:
            start.visited = True
            visited.append(start)

            if start == end:
                cleanup()
                return so_far

            for neigh in start.adj:
                node = neigh[0]
                weight = neigh[1]

                if node.visited:
                    continue

                pending.append(node, so_far + weight)

            while True:
                try:
                    n = pending.pop()
                except IndexError:
                    cleanup()
                    return -1

                if n[1].visited:
                    continue

                so_far = n[0]
                start = n[1]
                break


    def _run_testcase(self, name1, name2):
        n1 = self.node_lookup[name1]
        n2 = self.node_lookup[name2]

        print('{} -> {}: {}'.format(name1, name2, self._dijkstra(n1, n2)))

    def go(self):
        num_places = self._read_int()
        assert num_places > 0

        for i in range(num_places):
            self._add_place(i, self._read_string())

        num_links = self._read_int()
        assert num_links > 0

        for i in range(num_links):
            self._add_link(*self._read_link())

        while True:
            try:
                self._run_testcase(*self._read_testcase())
            except EOFError:
                return 0

def main():
    s = Scenario()
    s.go()

if __name__ =='__main__':
    main()
