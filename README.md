# Quantity

类型安全的物理量库，header-only，C++17。数值和单位绑在类型里，不同量纲不能混算；时间用 `std::chrono`，不另建时间单位。

---

## 用法

```cpp
#include "quantity.h"
#include <chrono>
#include <iostream>

using namespace nspt;
using namespace nspt::literals;
using namespace std::chrono_literals;

int main() {
    auto dist = 100.0_m + 2.0_km;             // 2100 m
    Kilometer km = dist;                      // 隐式换单位
    auto theta = 90.0_deg + 1.0_rad;

    auto v = 100.0_m / 2s;                    // 50 m/s
    auto v_kph = v.as<KilometerPerHour>();    // 180 km/h
    auto omega = 180.0_deg / 1s;
    auto omega_rad = omega.as<RadianPerSecond>();

    auto d2 = v * 3s;                         // 150 m
    auto dt = 150.0_m / v;                    // 3 s

    std::cout << to_string(v) << "  " << to_string(v_kph) << '\n';
}
```

线速度与角速度不能相加；`as` 也要求同量纲：

```cpp
// 100.0_m / 1s + 1.0_radps;           // 编译错误
// (100.0_m / 1s).as<RadianPerSecond>(); // 编译错误
```

### 字面量

| 类别 | 字面量 |
|------|--------|
| 长度 | `_m` `_km` `_mm` `_um` `_nm` `_cm` |
| 角度 | `_rad` `_deg` |
| 质量 | `_kg` |
| 线速度 | `_mps` `_mmps` `_umps` `_kph` |
| 角速度 | `_radps` `_degps` |

时间用标准库：`1s` `100ms` `2h`（`std::chrono_literals`）。

### 常用接口

| 接口 | 说明 |
|------|------|
| `Meter` 等别名 | `Quantity<UnitXxx, double>` |
| `Meter{v}` | 显式构造（裸数值） |
| `value()` | 取值 |
| `as<Kilometer>()` | 换到目标别名类型 |
| `Kilometer d = meter_q` | 同量纲隐式换单位 |
| `+` `-` 比较 | 同量纲；结果单位跟左边 |
| `*` `/` 标量 | 缩放 |
| `q / q` | 同量纲相除，得无量纲比值 |
| `q / duration` | → 速度 |
| `rate * duration` | → 位移 / 角位移 |
| `spatial / rate` | → `duration` |
| `unit_symbol<U>()` / `to_string(q)` | 符号 / `"数值 单位"` |

---

## 实现

单位类型为 `UnitXxx`，别名为 `Meter`、`MeterPerSecond` 等。

### Dimension

量纲标签在 `detail` 里：`length`、`angle`、`mass`、`linear_velocity`、`angular_velocity`。同一量纲可换算、比较。

### `unit_traits`

每个单位特化 `dimension`、`base_ratio`（相对该量纲基准的 `std::ratio`）和 `name()`：

```cpp
template <>
struct unit_traits<UnitMeter> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1>;
    static constexpr const char* name() { return "m"; }
};
```

| 量纲 | 基准 | 其它单位 |
|------|------|----------|
| length | `UnitMeter` | km、mm、um、nm、cm、in、ft |
| angle | `UnitRadian` | deg（经 `pi_ratio`） |
| mass | `UnitKilogram` | g |
| linear_velocity | m/s | mm/s、um/s、km/h |
| angular_velocity | rad/s | deg/s |

`is_unit_v<U>` 在 `unit_traits` 完整时为真。

### `Rate` 与 `Quantity`

```cpp
Rate<UnitMeter, std::ratio<1>>            // m/s
Rate<UnitKilometer, std::chrono::hours::period>  // km/h
```

`period` 是 `std::ratio`；`base_ratio = 空间单位 / period`。与具体 `duration` 的换算在运算符里做。

`Quantity<Unit, Rep>` 绑定数值与单位，默认 `Rep = double`。

### 扩展

特化 `unit_traits` 即可加自定义单位；长度或角度单位可直接写成 `Rate<MyUnit>` 或 `Rate<MyUnit, std::chrono::minutes::period>`。
