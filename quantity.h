#pragma once

#include <chrono>
#include <ratio>
#include <string>
#include <type_traits>

// =============================================================================
// Quantity — 类型安全物理量（header-only，C++17）
//
//   #include "quantity.h"
//   using namespace nspt;
//   using namespace nspt::literals;
//
//   auto d = 100.0_m + 2.0_km;              // 2100 m
//   Kilometer km = d;                       // 隐式换单位
//   auto v = 100.0_m / 2s;                  // 50 m/s
//   auto v_kph = v.as<KilometerPerHour>();  // 180 km/h
//   auto dist = v * 3s;                     // 150 m
//   auto dt = 150.0_m / v;                  // 3 s
//   to_string(v);                           // "50.000000 m/s"
// =============================================================================

namespace nspt {

// -----------------------------------------------------------------------------
// dimension 标签
// -----------------------------------------------------------------------------
namespace detail {

struct length {};
struct angle {};
struct mass {};
struct linear_velocity {};
struct angular_velocity {};

template <typename SpatialDimension>
struct rate_dimension;

template <>
struct rate_dimension<length> {
    using type = linear_velocity;
};

template <>
struct rate_dimension<angle> {
    using type = angular_velocity;
};

template <typename SpatialDimension>
using rate_dimension_t = typename rate_dimension<SpatialDimension>::type;

}  // namespace detail

// -----------------------------------------------------------------------------
// π 的有理近似
// -----------------------------------------------------------------------------
using pi_ratio = std::ratio<428224593349304, 136308121570117>;

// -----------------------------------------------------------------------------
// 单位萃取
// -----------------------------------------------------------------------------
template <typename Unit, typename Enable = void>
struct unit_traits;

template <typename Unit>
using dimension_of_t = typename unit_traits<Unit>::dimension;

template <typename Unit>
using base_ratio_t = typename unit_traits<Unit>::base_ratio;

template <typename U1, typename U2>
inline constexpr bool is_same_dimension_v =
    std::is_same_v<dimension_of_t<U1>, dimension_of_t<U2>>;

template <typename Unit>
inline constexpr bool is_length_v =
    std::is_same_v<dimension_of_t<Unit>, detail::length>;

template <typename Unit>
inline constexpr bool is_angle_v =
    std::is_same_v<dimension_of_t<Unit>, detail::angle>;

template <typename Unit>
inline constexpr bool is_spatial_v = is_length_v<Unit> || is_angle_v<Unit>;

template <typename ToUnit, typename FromUnit, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<FromUnit, ToUnit>>>
constexpr Rep unit_cast(const Rep& value) noexcept {
    using Factor =
        std::ratio_divide<base_ratio_t<FromUnit>, base_ratio_t<ToUnit>>;
    return static_cast<Rep>(
        value * static_cast<Rep>(Factor::num) / static_cast<Rep>(Factor::den));
}

// -----------------------------------------------------------------------------
// 长度单位
// -----------------------------------------------------------------------------
struct UnitMeter {};
struct UnitKilometer {};
struct UnitMillimeter {};
struct UnitMicrometer {};
struct UnitNanometer {};
struct UnitCentimeter {};
struct UnitInch {};
struct UnitFoot {};

template <>
struct unit_traits<UnitMeter> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1>;
    static constexpr const char* name() { return "m"; }
};

template <>
struct unit_traits<UnitKilometer> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1000>;
    static constexpr const char* name() { return "km"; }
};

template <>
struct unit_traits<UnitMillimeter> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1, 1000>;
    static constexpr const char* name() { return "mm"; }
};

template <>
struct unit_traits<UnitMicrometer> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1, 1000000>;
    static constexpr const char* name() { return "um"; }
};

template <>
struct unit_traits<UnitNanometer> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1, 1000000000>;
    static constexpr const char* name() { return "nm"; }
};

template <>
struct unit_traits<UnitCentimeter> {
    using dimension = detail::length;
    using base_ratio = std::ratio<1, 100>;
    static constexpr const char* name() { return "cm"; }
};

template <>
struct unit_traits<UnitInch> {
    using dimension = detail::length;
    using base_ratio = std::ratio<254, 10000>;
    static constexpr const char* name() { return "in"; }
};

template <>
struct unit_traits<UnitFoot> {
    using dimension = detail::length;
    using base_ratio = std::ratio<3048, 10000>;
    static constexpr const char* name() { return "ft"; }
};

// -----------------------------------------------------------------------------
// 角度单位
// -----------------------------------------------------------------------------
struct UnitRadian {};
struct UnitDegree {};

template <>
struct unit_traits<UnitRadian> {
    using dimension = detail::angle;
    using base_ratio = std::ratio<1>;
    static constexpr const char* name() { return "rad"; }
};

template <>
struct unit_traits<UnitDegree> {
    using dimension = detail::angle;
    using base_ratio = std::ratio_divide<pi_ratio, std::ratio<180>>;
    static constexpr const char* name() { return "deg"; }
};

// -----------------------------------------------------------------------------
// 质量单位
// -----------------------------------------------------------------------------
struct UnitKilogram {};
struct UnitGram {};

template <>
struct unit_traits<UnitKilogram> {
    using dimension = detail::mass;
    using base_ratio = std::ratio<1>;
    static constexpr const char* name() { return "kg"; }
};

template <>
struct unit_traits<UnitGram> {
    using dimension = detail::mass;
    using base_ratio = std::ratio<1, 1000>;
    static constexpr const char* name() { return "g"; }
};

// -----------------------------------------------------------------------------
// 速度：Rate<SpatialUnit, period>（period 为 std::ratio，如 std::ratio<1> 表示 /s）
// -----------------------------------------------------------------------------
template <typename SpatialUnit, typename period = std::ratio<1>, typename = void>
struct Rate;

template <typename SpatialUnit, std::intmax_t Num, std::intmax_t Den>
struct Rate<SpatialUnit, std::ratio<Num, Den>,
            std::enable_if_t<is_spatial_v<SpatialUnit>>> {
    using spatial_unit = SpatialUnit;
    using period       = std::ratio<Num, Den>;
};

template <typename SpatialUnit, std::intmax_t Num, std::intmax_t Den>
struct unit_traits<Rate<SpatialUnit, std::ratio<Num, Den>>,
                   std::enable_if_t<is_spatial_v<SpatialUnit>>> {
    using period = std::ratio<Num, Den>;
    using dimension = detail::rate_dimension_t<dimension_of_t<SpatialUnit>>;
    using base_ratio = std::ratio_divide<base_ratio_t<SpatialUnit>, period>;
};

using UnitMeterPerSecond       = Rate<UnitMeter, std::ratio<1>>;
using UnitMillimeterPerSecond  = Rate<UnitMillimeter, std::ratio<1>>;
using UnitMicrometerPerSecond  = Rate<UnitMicrometer, std::ratio<1>>;
using UnitKilometerPerHour     = Rate<UnitKilometer, std::chrono::hours::period>;
using UnitRadianPerSecond      = Rate<UnitRadian, std::ratio<1>>;
using UnitDegreePerSecond      = Rate<UnitDegree, std::ratio<1>>;

// -----------------------------------------------------------------------------
// is_unit_v
// -----------------------------------------------------------------------------
namespace detail {

template <typename Unit, typename = void>
struct is_unit_impl : std::false_type {};

template <typename Unit>
struct is_unit_impl<
    Unit, std::void_t<typename unit_traits<Unit>::dimension,
                      typename unit_traits<Unit>::base_ratio>> : std::true_type {};

}  // namespace detail

template <typename Unit>
inline constexpr bool is_unit_v = detail::is_unit_impl<Unit>::value;

// -----------------------------------------------------------------------------
// Quantity<Unit, Rep>
// -----------------------------------------------------------------------------
template <typename Unit, typename Rep = double,
          typename = std::enable_if_t<is_unit_v<Unit>>>
class Quantity {
public:
    using unit_type       = Unit;
    using rep             = Rep;
    using value_type      = Rep;
    using dimension_type  = dimension_of_t<Unit>;

    constexpr Quantity() noexcept : value_(Rep{}) {}
    constexpr explicit Quantity(const Rep& v) noexcept : value_(v) {}

    template <typename OtherUnit,
              typename = std::enable_if_t<
                  is_same_dimension_v<Unit, OtherUnit> &&
                  !std::is_same_v<Unit, OtherUnit>>>
    constexpr Quantity(const Quantity<OtherUnit, Rep>& other) noexcept
        : value_(unit_cast<Unit, OtherUnit>(other.value())) {}

    template <typename OtherUnit,
              typename = std::enable_if_t<
                  is_same_dimension_v<Unit, OtherUnit> &&
                  !std::is_same_v<Unit, OtherUnit>>>
    constexpr Quantity& operator=(const Quantity<OtherUnit, Rep>& other) noexcept {
        value_ = unit_cast<Unit, OtherUnit>(other.value());
        return *this;
    }

    constexpr const Rep& value() const noexcept { return value_; }

    constexpr Quantity operator+() const noexcept { return *this; }
    constexpr Quantity operator-() const noexcept { return Quantity(-value_); }

    template <typename OtherUnit,
              typename = std::enable_if_t<is_same_dimension_v<Unit, OtherUnit>>>
    constexpr Quantity& operator+=(const Quantity<OtherUnit, Rep>& rhs) noexcept {
        value_ += unit_cast<Unit, OtherUnit>(rhs.value());
        return *this;
    }
    template <typename OtherUnit,
              typename = std::enable_if_t<is_same_dimension_v<Unit, OtherUnit>>>
    constexpr Quantity& operator-=(const Quantity<OtherUnit, Rep>& rhs) noexcept {
        value_ -= unit_cast<Unit, OtherUnit>(rhs.value());
        return *this;
    }
    constexpr Quantity& operator*=(const Rep& s) noexcept {
        value_ *= s;
        return *this;
    }
    constexpr Quantity& operator/=(const Rep& s) noexcept {
        value_ /= s;
        return *this;
    }

    template <typename OtherQuantity,
              typename = std::enable_if_t<is_same_dimension_v<Unit, typename OtherQuantity::unit_type>>>
    constexpr OtherQuantity as() const noexcept {
        using OtherRep = typename OtherQuantity::rep;
        return OtherQuantity(static_cast<OtherRep>(
            unit_cast<typename OtherQuantity::unit_type, Unit>(value_)));
    }

    template <typename OtherUnit,
              typename = std::enable_if_t<is_same_dimension_v<Unit, OtherUnit>>>
    constexpr int compare(const Quantity<OtherUnit, Rep>& rhs) const noexcept {
        using BaseFrom = base_ratio_t<Unit>;
        using BaseTo   = base_ratio_t<OtherUnit>;
        const Rep a = static_cast<Rep>(
            value_ * static_cast<Rep>(BaseFrom::num) / static_cast<Rep>(BaseFrom::den));
        const Rep b = static_cast<Rep>(
            rhs.value() * static_cast<Rep>(BaseTo::num) / static_cast<Rep>(BaseTo::den));
        if (a < b) return -1;
        if (a > b) return 1;
        return 0;
    }

private:
    Rep value_;
};

// -----------------------------------------------------------------------------
// 算术运算
// -----------------------------------------------------------------------------
template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr Quantity<U1, Rep>
operator+(Quantity<U1, Rep> a, const Quantity<U2, Rep>& b) noexcept {
    return a += b;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr Quantity<U1, Rep>
operator-(Quantity<U1, Rep> a, const Quantity<U2, Rep>& b) noexcept {
    return a -= b;
}

template <typename Unit, typename Rep>
constexpr Quantity<Unit, Rep>
operator*(Quantity<Unit, Rep> q, const Rep& s) noexcept {
    return q *= s;
}

template <typename Unit, typename Rep>
constexpr Quantity<Unit, Rep>
operator*(const Rep& s, Quantity<Unit, Rep> q) noexcept {
    return q *= s;
}

template <typename Unit, typename Rep>
constexpr Quantity<Unit, Rep>
operator/(Quantity<Unit, Rep> q, const Rep& s) noexcept {
    return q /= s;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr Rep
operator/(const Quantity<U1, Rep>& a, const Quantity<U2, Rep>& b) noexcept {
    return a.value() / unit_cast<U1, U2>(b.value());
}

// -----------------------------------------------------------------------------
// 与 std::chrono
// -----------------------------------------------------------------------------
template <typename SpatialUnit, typename Rep, typename DurRep, typename period,
          typename = std::enable_if_t<is_spatial_v<SpatialUnit>>>
constexpr Quantity<Rate<SpatialUnit, period>, Rep>
operator/(const Quantity<SpatialUnit, Rep>& q,
          const std::chrono::duration<DurRep, period>& dt) noexcept {
    return Quantity<Rate<SpatialUnit, period>, Rep>(
        q.value() / static_cast<Rep>(dt.count()));
}

template <typename SpatialUnit, typename rate_period, typename Rep,
          typename DurRep, typename dur_period>
constexpr Quantity<SpatialUnit, Rep>
operator*(const Quantity<Rate<SpatialUnit, rate_period>, Rep>& rate,
          const std::chrono::duration<DurRep, dur_period>& dt) noexcept {
    using CountDur = std::chrono::duration<Rep, rate_period>;
    const CountDur d = std::chrono::duration_cast<CountDur>(dt);
    return Quantity<SpatialUnit, Rep>(rate.value() * d.count());
}

template <typename SpatialUnit, typename rate_period, typename Rep,
          typename DurRep, typename dur_period>
constexpr Quantity<SpatialUnit, Rep>
operator*(const std::chrono::duration<DurRep, dur_period>& dt,
          const Quantity<Rate<SpatialUnit, rate_period>, Rep>& rate) noexcept {
    return rate * dt;
}

template <typename SpatialUnit, typename rate_period, typename Rep>
constexpr std::chrono::duration<Rep, rate_period>
operator/(const Quantity<SpatialUnit, Rep>& spatial,
          const Quantity<Rate<SpatialUnit, rate_period>, Rep>& rate) noexcept {
    return std::chrono::duration<Rep, rate_period>(spatial.value() / rate.value());
}

// -----------------------------------------------------------------------------
// 比较
// -----------------------------------------------------------------------------
template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr bool operator==(const Quantity<U1, Rep>& a,
                          const Quantity<U2, Rep>& b) noexcept {
    return a.compare(b) == 0;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr bool operator!=(const Quantity<U1, Rep>& a,
                          const Quantity<U2, Rep>& b) noexcept {
    return a.compare(b) != 0;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr bool operator<(const Quantity<U1, Rep>& a,
                         const Quantity<U2, Rep>& b) noexcept {
    return a.compare(b) < 0;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr bool operator>(const Quantity<U1, Rep>& a,
                         const Quantity<U2, Rep>& b) noexcept {
    return a.compare(b) > 0;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr bool operator<=(const Quantity<U1, Rep>& a,
                          const Quantity<U2, Rep>& b) noexcept {
    return a.compare(b) <= 0;
}

template <typename U1, typename U2, typename Rep,
          typename = std::enable_if_t<is_same_dimension_v<U1, U2>>>
constexpr bool operator>=(const Quantity<U1, Rep>& a,
                          const Quantity<U2, Rep>& b) noexcept {
    return a.compare(b) >= 0;
}

// -----------------------------------------------------------------------------
// 字符串转换
// -----------------------------------------------------------------------------

template <typename period>
inline std::string period_symbol() {
    if constexpr (std::ratio_equal_v<period, std::ratio<1>>) {
        return "s";
    } else if constexpr (std::ratio_equal_v<period, std::milli>) {
        return "ms";
    } else if constexpr (std::ratio_equal_v<period, std::micro>) {
        return "us";
    } else if constexpr (std::ratio_equal_v<period, std::nano>) {
        return "ns";
    } else if constexpr (std::ratio_equal_v<period, std::ratio<60>>) {
        return "min";
    } else if constexpr (std::ratio_equal_v<period, std::ratio<3600>>) {
        return "h";
    } else if constexpr (std::ratio_equal_v<period, std::ratio<86400>>) {
        return "d";
    } else {
        std::string s = "(";
        s += std::to_string(period::num);
        if constexpr (period::den != 1) {
            s += '/';
            s += std::to_string(period::den);
        }
        s += " s)";
        return s;
    }
}

namespace detail {

template <typename Unit, typename = void>
struct unit_symbol_impl {
    static std::string get() { return unit_traits<Unit>::name(); }
};

template <typename SpatialUnit, std::intmax_t Num, std::intmax_t Den>
struct unit_symbol_impl<Rate<SpatialUnit, std::ratio<Num, Den>>, void> {
    static std::string get() {
        return std::string(unit_traits<SpatialUnit>::name()) + "/" +
               period_symbol<std::ratio<Num, Den>>();
    }
};

}  // namespace detail

template <typename Unit>
inline std::string unit_symbol() {
    return detail::unit_symbol_impl<Unit>::get();
}

template <typename Unit, typename Rep>
inline std::string to_string(const Quantity<Unit, Rep>& q) {
    return std::to_string(q.value()) + " " + unit_symbol<Unit>();
}

// -----------------------------------------------------------------------------
// 别名（Rep = double）
// -----------------------------------------------------------------------------
using Meter               = Quantity<UnitMeter>;
using Kilometer           = Quantity<UnitKilometer>;
using Millimeter          = Quantity<UnitMillimeter>;
using Micrometer          = Quantity<UnitMicrometer>;
using Nanometer           = Quantity<UnitNanometer>;
using Centimeter          = Quantity<UnitCentimeter>;
using Inch                = Quantity<UnitInch>;
using Foot                = Quantity<UnitFoot>;
using Radian              = Quantity<UnitRadian>;
using Degree              = Quantity<UnitDegree>;
using Kilogram            = Quantity<UnitKilogram>;
using Gram                = Quantity<UnitGram>;
using MeterPerSecond      = Quantity<UnitMeterPerSecond>;
using MillimeterPerSecond = Quantity<UnitMillimeterPerSecond>;
using MicrometerPerSecond = Quantity<UnitMicrometerPerSecond>;
using KilometerPerHour    = Quantity<UnitKilometerPerHour>;
using RadianPerSecond     = Quantity<UnitRadianPerSecond>;
using DegreePerSecond     = Quantity<UnitDegreePerSecond>;

// -----------------------------------------------------------------------------
// 字面量
// -----------------------------------------------------------------------------
namespace literals {

constexpr Meter operator""_m(long double v) noexcept {
    return Meter{static_cast<double>(v)};
}
constexpr Meter operator""_m(unsigned long long v) noexcept {
    return Meter{static_cast<double>(v)};
}
constexpr Kilometer operator""_km(long double v) noexcept {
    return Kilometer{static_cast<double>(v)};
}
constexpr Kilometer operator""_km(unsigned long long v) noexcept {
    return Kilometer{static_cast<double>(v)};
}
constexpr Millimeter operator""_mm(long double v) noexcept {
    return Millimeter{static_cast<double>(v)};
}
constexpr Millimeter operator""_mm(unsigned long long v) noexcept {
    return Millimeter{static_cast<double>(v)};
}
constexpr Micrometer operator""_um(long double v) noexcept {
    return Micrometer{static_cast<double>(v)};
}
constexpr Micrometer operator""_um(unsigned long long v) noexcept {
    return Micrometer{static_cast<double>(v)};
}
constexpr Nanometer operator""_nm(long double v) noexcept {
    return Nanometer{static_cast<double>(v)};
}
constexpr Nanometer operator""_nm(unsigned long long v) noexcept {
    return Nanometer{static_cast<double>(v)};
}
constexpr Centimeter operator""_cm(long double v) noexcept {
    return Centimeter{static_cast<double>(v)};
}
constexpr Centimeter operator""_cm(unsigned long long v) noexcept {
    return Centimeter{static_cast<double>(v)};
}

constexpr Radian operator""_rad(long double v) noexcept {
    return Radian{static_cast<double>(v)};
}
constexpr Radian operator""_rad(unsigned long long v) noexcept {
    return Radian{static_cast<double>(v)};
}
constexpr Degree operator""_deg(long double v) noexcept {
    return Degree{static_cast<double>(v)};
}
constexpr Degree operator""_deg(unsigned long long v) noexcept {
    return Degree{static_cast<double>(v)};
}

constexpr Kilogram operator""_kg(long double v) noexcept {
    return Kilogram{static_cast<double>(v)};
}
constexpr Kilogram operator""_kg(unsigned long long v) noexcept {
    return Kilogram{static_cast<double>(v)};
}

constexpr MeterPerSecond operator""_mps(long double v) noexcept {
    return MeterPerSecond{static_cast<double>(v)};
}
constexpr MeterPerSecond operator""_mps(unsigned long long v) noexcept {
    return MeterPerSecond{static_cast<double>(v)};
}
constexpr MillimeterPerSecond operator""_mmps(long double v) noexcept {
    return MillimeterPerSecond{static_cast<double>(v)};
}
constexpr MillimeterPerSecond operator""_mmps(unsigned long long v) noexcept {
    return MillimeterPerSecond{static_cast<double>(v)};
}
constexpr MicrometerPerSecond operator""_umps(long double v) noexcept {
    return MicrometerPerSecond{static_cast<double>(v)};
}
constexpr MicrometerPerSecond operator""_umps(unsigned long long v) noexcept {
    return MicrometerPerSecond{static_cast<double>(v)};
}
constexpr KilometerPerHour operator""_kph(long double v) noexcept {
    return KilometerPerHour{static_cast<double>(v)};
}
constexpr KilometerPerHour operator""_kph(unsigned long long v) noexcept {
    return KilometerPerHour{static_cast<double>(v)};
}
constexpr RadianPerSecond operator""_radps(long double v) noexcept {
    return RadianPerSecond{static_cast<double>(v)};
}
constexpr RadianPerSecond operator""_radps(unsigned long long v) noexcept {
    return RadianPerSecond{static_cast<double>(v)};
}
constexpr DegreePerSecond operator""_degps(long double v) noexcept {
    return DegreePerSecond{static_cast<double>(v)};
}
constexpr DegreePerSecond operator""_degps(unsigned long long v) noexcept {
    return DegreePerSecond{static_cast<double>(v)};
}

}  // namespace literals

}  // namespace nspt
