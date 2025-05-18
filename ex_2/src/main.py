import numpy as np

class Banker:
    def __init__(self, num_processes, num_resources, total_resources, allocation, max):
        self.num_processes = num_processes
        self.num_resources = num_resources
        self.total_resources = np.array(total_resources)
        self.allocation = np.array(allocation) # 分配矩阵
        self.max = np.array(max) # 最大需求矩阵
        self.available = self.total_resources - np.sum(self.allocation, axis=0) #可利用资源向量
        self.need = self.max - self.allocation # 需求矩阵

    # 检查系统是否处于安全状态    
    def is_safe_state(self):
        work = self.available.copy() #动态可分配资源
        finish = [False] * self.num_processes # 是否有足够的资源分配给进程，初值为 false
        safe_sequence = []
        
        while True:
            found = False
            for i in range(self.num_processes):
                if not finish[i] and np.all(self.need[i] <= work):
                    work += self.allocation[i]
                    finish[i] = True
                    safe_sequence.append(i)
                    found = True
                    break
            if not found:
                break
                
        if all(finish):
            return True, safe_sequence
        else:
            return False, []
    
    # 第 process_id 进程请求资源
    def request_resources(self, process_id, request): 
        request = np.array(request)
        
        # 检查请求是否超过需求
        if np.any(request > self.need[process_id]):
            print(f"错误：进程 {process_id} 的请求超过了其最大需求")
            return False
            
        # 检查请求是否超过可用资源
        if np.any(request > self.available):
            print(f"错误：进程 {process_id} 的请求超过了可用资源")
            return False
            
        # 尝试分配资源
        old_available = self.available.copy()
        old_allocation = self.allocation.copy()
        old_need = self.need.copy()
        
        self.available -= request
        self.allocation[process_id] += request
        self.need[process_id] -= request
        
        # 检查安全性
        safe, sequence = self.is_safe_state()
        
        if safe:
            print(f"资源分配成功，安全序列为: {sequence}")
            return True
        else:
            print(f"资源分配会导致不安全状态，请求被拒绝")
            # 回滚分配
            self.available = old_available
            self.allocation = old_allocation
            self.need = old_need
            return False
    
    def display_state(self):
        print("\n-> 当前系统状态:")
        print("可用资源:", self.available)
        print("\n分配矩阵:")
        print("进程\t", end="")
        for j in range(self.num_resources):
            print(f"资源{j}\t", end="")
        print()
        for i in range(self.num_processes):
            print(f"{i}\t", end="")
            for j in range(self.num_resources):
                print(f"{self.allocation[i][j]}\t", end="")
            print()
            
        print("\n需求矩阵:")
        print("进程\t", end="")
        for j in range(self.num_resources):
            print(f"资源{j}\t", end="")
        print()
        for i in range(self.num_processes):
            print(f"{i}\t", end="")
            for j in range(self.num_resources):
                print(f"{self.need[i][j]}\t", end="")
            print()
            
        safe, sequence = self.is_safe_state()
        if safe:
            print(f"\n系统处于安全状态，安全序列为: {sequence}")
        else:
            print("\n系统处于不安全状态")


def run_test_case(test_case):
    print(f"\n===== 测试用例 {test_case['name']} =====")
    
    banker = Banker(
        num_processes=test_case['num_processes'],
        num_resources=test_case['num_resources'],
        total_resources=test_case['total_resources'],
        allocation=test_case['allocation'],
        max=test_case['max']
    )
    
    banker.display_state()
    
    for request in test_case['requests']:
        process_id, request_resources = request
        print(f"\n进程 {process_id} 请求资源: {request_resources}")
        banker.request_resources(process_id, request_resources)
        banker.display_state()


# 测试用例
test_cases = [
    {
        'name': '资源分配成功案例',
        'num_processes': 5,
        'num_resources': 3,
        'total_resources': [10, 5, 7],
        'allocation': [
            [0, 1, 0],
            [2, 0, 0],
            [3, 0, 2],
            [2, 1, 1],
            [0, 0, 2]
        ],
        'max': [
            [7, 5, 3],
            [3, 2, 2],
            [9, 0, 2],
            [2, 2, 2],
            [4, 3, 3]
        ],
        'requests': [
            (1, [1, 0, 2]),
            (4, [3, 3, 0]),
            (0, [0, 2, 0])
        ]
    },
    {
        'name': '资源分配失败案例',
        'num_processes': 5,
        'num_resources': 3,
        'total_resources': [10, 5, 7],
        'allocation': [
            [0, 4, 0],
            [2, 0, 0],
            [3, 0, 2],
            [2, 1, 1],
            [0, 0, 2]
        ],
        'max': [
            [7, 5, 3],
            [3, 2, 2],
            [9, 0, 2],
            [2, 2, 2],
            [4, 3, 3]
        ],
        'requests': [
            (1, [3, 0, 2]),  # 超过需求
            (4, [3, 0, 3]),  # 超过可用资源
            (0, [0, 0, 2]),  # 导致不安全状态
        ]
    },
    {
        'name': '多资源类型案例',
        'num_processes': 4,
        'num_resources': 4,
        'total_resources': [10, 5, 7, 8],
        'allocation': [
            [0, 1, 1, 0],
            [2, 0, 0, 1],
            [3, 0, 2, 0],
            [2, 1, 1, 2]
        ],
        'max': [
            [7, 5, 3, 2],
            [3, 2, 2, 2],
            [9, 0, 2, 2],
            [2, 2, 2, 4]
        ],
        'requests': [
            (1, [1, 0, 1, 0]),
            (3, [0, 1, 0, 1]),
            (2, [2, 0, 0, 1])
        ]
    }
]

# 运行所有测试用例
for test_case in test_cases:
    run_test_case(test_case)    