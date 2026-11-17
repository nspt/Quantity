#include "quantity.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ratio>
#include <string>
#include <type_traits>

using namespace nspt;
using namespace nspt::literals;
using namespace std::chrono_literals;

namespace {

int g_failed = 0;

void check(bool ok, const char* expr, const char* file, int line) {
    if (!ok) {
        std::cerr << "FAIL  " << file << ':' << line << "  " << expr << '\n';
        ++g_failed;
    } else {
        std::cout << "ok    " << expr << '\n';
    }
}

void check_near(double a, double b, double tol, const char* expr,
                const char* file, int line) {
    check(std::abs(a - b) <= tol, expr, file, line);
}

}  // namespace

#define EXPECT(expr) check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
#define EXPECT_NEAR(a, b, tol) \
    check_near((a), (b), (tol), #a " ≈ " #b, __FILE__, __LINE__)

static void test_dimensions() {
    std::cout << "\n== dimension / unit traits ==\n";

    // length
    EXPECT((std::is_same_v<dimension_of_t<UnitNanometer>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitMicrometer>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitMillimeter>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitCentimeter>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitInch>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitFoot>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitMeter>, detail::length>));
    EXPECT((std::is_same_v<dimension_of_t<UnitKilometer>, detail::length>));

    // angle
    EXPECT((std::is_same_v<dimension_of_t<UnitRadian>, detail::angle>));
    EXPECT((std::is_same_v<dimension_of_t<UnitDegree>, detail::angle>));

    // mass
    EXPECT((std::is_same_v<dimension_of_t<UnitKilogram>, detail::mass>));
    EXPECT((std::is_same_v<dimension_of_t<UnitGram>, detail::mass>));

    // rates
    EXPECT((std::is_same_v<dimension_of_t<UnitMicrometerPerSecond>, detail::linear_velocity>));
    EXPECT((std::is_same_v<dimension_of_t<UnitMillimeterPerSecond>, detail::linear_velocity>));
    EXPECT((std::is_same_v<dimension_of_t<UnitKilometerPerHour>, detail::linear_velocity>));
    EXPECT((std::is_same_v<dimension_of_t<UnitMeterPerSecond>, detail::linear_velocity>));
    EXPECT((std::is_same_v<dimension_of_t<Rate<UnitMeter, std::milli>>, detail::linear_velocity>));
    EXPECT((std::is_same_v<dimension_of_t<UnitDegreePerSecond>, detail::angular_velocity>));
    EXPECT((std::is_same_v<dimension_of_t<UnitRadianPerSecond>, detail::angular_velocity>));

    EXPECT((is_same_dimension_v<UnitMeter, UnitKilometer>));
    EXPECT((is_same_dimension_v<UnitMeter, UnitInch>));
    EXPECT((is_same_dimension_v<UnitRadian, UnitDegree>));
    EXPECT((is_same_dimension_v<UnitKilogram, UnitGram>));
    EXPECT((is_same_dimension_v<UnitMeterPerSecond, UnitKilometerPerHour>));
    EXPECT((is_same_dimension_v<UnitRadianPerSecond, UnitDegreePerSecond>));
    EXPECT((!is_same_dimension_v<UnitMeter, UnitRadian>));
    EXPECT((!is_same_dimension_v<UnitMeter, UnitKilogram>));
    EXPECT((!is_same_dimension_v<UnitMeterPerSecond, UnitRadianPerSecond>));

    EXPECT(is_length_v<UnitMeter>);
    EXPECT(is_length_v<UnitFoot>);
    EXPECT((!is_length_v<UnitRadian>));
    EXPECT(is_angle_v<UnitDegree>);
    EXPECT((!is_angle_v<UnitMeter>));
    EXPECT(is_spatial_v<UnitMeter>);
    EXPECT(is_spatial_v<UnitRadian>);
    EXPECT((!is_spatial_v<UnitKilogram>));
    EXPECT((!is_spatial_v<UnitMeterPerSecond>));

    EXPECT((std::ratio_equal_v<base_ratio_t<UnitMeter>, std::ratio<1>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitKilometer>, std::ratio<1000>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitMillimeter>, std::ratio<1, 1000>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitCentimeter>, std::ratio<1, 100>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitInch>, std::ratio<254, 10000>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitGram>, std::ratio<1, 1000>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitMeterPerSecond>, std::ratio<1>>));
    EXPECT((std::ratio_equal_v<base_ratio_t<UnitKilometerPerHour>,
                               std::ratio_divide<std::ratio<1000>, std::ratio<3600>>>));

    EXPECT(is_unit_v<UnitMeter>);
    EXPECT(is_unit_v<UnitKilometer>);
    EXPECT(is_unit_v<UnitMillimeter>);
    EXPECT(is_unit_v<UnitMicrometer>);
    EXPECT(is_unit_v<UnitNanometer>);
    EXPECT(is_unit_v<UnitCentimeter>);
    EXPECT(is_unit_v<UnitInch>);
    EXPECT(is_unit_v<UnitFoot>);
    EXPECT(is_unit_v<UnitRadian>);
    EXPECT(is_unit_v<UnitDegree>);
    EXPECT(is_unit_v<UnitKilogram>);
    EXPECT(is_unit_v<UnitGram>);
    EXPECT(is_unit_v<UnitMeterPerSecond>);
    EXPECT(is_unit_v<UnitMillimeterPerSecond>);
    EXPECT(is_unit_v<UnitMicrometerPerSecond>);
    EXPECT(is_unit_v<UnitKilometerPerHour>);
    EXPECT(is_unit_v<UnitRadianPerSecond>);
    EXPECT(is_unit_v<UnitDegreePerSecond>);
    EXPECT((is_unit_v<Rate<UnitMeter, std::milli>>));
    EXPECT((!is_unit_v<int>));
    EXPECT((!is_unit_v<detail::length>));
    EXPECT((!is_unit_v<Meter>));  // Quantity 本身不是 unit

    EXPECT_NEAR((unit_cast<UnitKilometer, UnitMeter>(1000.0)), 1.0, 1e-12);
    EXPECT_NEAR((unit_cast<UnitMeter, UnitCentimeter>(100.0)), 1.0, 1e-12);
    EXPECT_NEAR((unit_cast<UnitGram, UnitKilogram>(1.0)), 1000.0, 1e-12);
}

static void test_literals_and_aliases() {
    std::cout << "\n== literals return aliases ==\n";

    EXPECT((std::is_same_v<decltype(3.0_m), Meter>));
    EXPECT((std::is_same_v<decltype(3_m), Meter>));
    EXPECT((std::is_same_v<decltype(1.5_km), Kilometer>));
    EXPECT((std::is_same_v<decltype(2_km), Kilometer>));
    EXPECT((std::is_same_v<decltype(10.0_mm), Millimeter>));
    EXPECT((std::is_same_v<decltype(10_mm), Millimeter>));
    EXPECT((std::is_same_v<decltype(1.0_um), Micrometer>));
    EXPECT((std::is_same_v<decltype(1_um), Micrometer>));
    EXPECT((std::is_same_v<decltype(1.0_nm), Nanometer>));
    EXPECT((std::is_same_v<decltype(1_nm), Nanometer>));
    EXPECT((std::is_same_v<decltype(100.0_cm), Centimeter>));
    EXPECT((std::is_same_v<decltype(100_cm), Centimeter>));
    EXPECT((std::is_same_v<decltype(1.0_rad), Radian>));
    EXPECT((std::is_same_v<decltype(1_rad), Radian>));
    EXPECT((std::is_same_v<decltype(90.0_deg), Degree>));
    EXPECT((std::is_same_v<decltype(90_deg), Degree>));
    EXPECT((std::is_same_v<decltype(2.5_kg), Kilogram>));
    EXPECT((std::is_same_v<decltype(2_kg), Kilogram>));
    EXPECT((std::is_same_v<decltype(50.0_mps), MeterPerSecond>));
    EXPECT((std::is_same_v<decltype(50_mps), MeterPerSecond>));
    EXPECT((std::is_same_v<decltype(1.0_mmps), MillimeterPerSecond>));
    EXPECT((std::is_same_v<decltype(1_mmps), MillimeterPerSecond>));
    EXPECT((std::is_same_v<decltype(1.0_umps), MicrometerPerSecond>));
    EXPECT((std::is_same_v<decltype(1_umps), MicrometerPerSecond>));
    EXPECT((std::is_same_v<decltype(36.0_kph), KilometerPerHour>));
    EXPECT((std::is_same_v<decltype(36_kph), KilometerPerHour>));
    EXPECT((std::is_same_v<decltype(1.0_radps), RadianPerSecond>));
    EXPECT((std::is_same_v<decltype(1_radps), RadianPerSecond>));
    EXPECT((std::is_same_v<decltype(90.0_degps), DegreePerSecond>));
    EXPECT((std::is_same_v<decltype(90_degps), DegreePerSecond>));

    EXPECT((std::is_same_v<Meter, Quantity<UnitMeter, double>>));
    EXPECT((std::is_same_v<Gram, Quantity<UnitGram, double>>));
    EXPECT((std::is_same_v<MeterPerSecond, Quantity<UnitMeterPerSecond, double>>));
}

static void test_length_and_same_dimension_arith() {
    std::cout << "\n== length / same-dimension arithmetic ==\n";

    auto a = 3.0_m;
    auto b = 1.5_km;
    EXPECT(a.value() == 3.0);
    EXPECT(b.value() == 1.5);

    auto a_km = a.as<Kilometer>();
    EXPECT_NEAR(a_km.value(), 0.003, 1e-12);

    auto sum = a + b;
    EXPECT((std::is_same_v<decltype(sum), Meter>));
    EXPECT_NEAR(sum.value(), 1503.0, 1e-9);

    auto diff = b - a;
    EXPECT((std::is_same_v<decltype(diff), Kilometer>));
    EXPECT_NEAR(diff.value(), 1.497, 1e-9);

    EXPECT(b > a);
    EXPECT(a < b);
    EXPECT(a <= a);
    EXPECT(b >= a);
    EXPECT(3.0_m == 3000.0_mm);
    EXPECT(1.0_mm == 1000.0_um);
    EXPECT(1.0_um == 1000.0_nm);
    EXPECT(100.0_cm == 1.0_m);
    EXPECT(100_cm == 1_m);
    EXPECT(1.0_m != 1.0_km);

    EXPECT(Inch{12.0} == Foot{1.0});
    EXPECT_NEAR(Inch{1.0}.as<Meter>().value(), 0.0254, 1e-12);
    EXPECT_NEAR(Foot{1.0}.as<Meter>().value(), 0.3048, 1e-12);

    EXPECT_NEAR((100.0_m / 2.0_km), 0.05, 1e-12);
    EXPECT_NEAR((a * 2.0).value(), 6.0, 1e-12);
    EXPECT_NEAR((2.0 * a).value(), 6.0, 1e-12);
    EXPECT_NEAR((a / 2.0).value(), 1.5, 1e-12);

    Quantity<UnitMeter, float> f{2.0f};
    EXPECT_NEAR(static_cast<double>(f.value()), 2.0, 1e-6);
}

static void test_angle_and_pi_ratio() {
    std::cout << "\n== angle ==\n";

    auto half_turn = (180.0_deg).as<Radian>();
    EXPECT_NEAR(half_turn.value(),
                static_cast<double>(pi_ratio::num) / pi_ratio::den,
                1e-12);

    auto mixed = 90.0_deg + 1.0_rad;
    EXPECT((std::is_same_v<decltype(mixed), Degree>));
    EXPECT_NEAR(mixed.as<Radian>().value(),
                half_turn.value() / 2.0 + 1.0, 1e-9);

    EXPECT(180_deg == Radian{static_cast<double>(pi_ratio::num) / pi_ratio::den});
}

static void test_linear_velocity() {
    std::cout << "\n== linear velocity (chrono) ==\n";

    auto v = 100.0_m / 2s;
    EXPECT((std::is_same_v<decltype(v), MeterPerSecond>));
    EXPECT_NEAR(v.value(), 50.0, 1e-12);

    auto v_kph = v.as<KilometerPerHour>();
    EXPECT_NEAR(v_kph.value(), 180.0, 1e-9);

    auto d = v * 3s;
    EXPECT_NEAR(d.value(), 150.0, 1e-9);

    auto d2 = 3s * v;
    EXPECT_NEAR(d2.value(), 150.0, 1e-9);

    auto dt = 150.0_m / v;
    EXPECT_NEAR(dt.count(), 3.0, 1e-9);

    auto sum_v = 10.0_mps + 36.0_kph;  // 10 + 10 = 20 m/s
    EXPECT((std::is_same_v<decltype(sum_v), MeterPerSecond>));
    EXPECT_NEAR(sum_v.value(), 20.0, 1e-9);

    EXPECT(50.0_mps == 180.0_kph);
    EXPECT(1.0_mps == 1000.0_mmps);
    EXPECT(1.0_mmps == 1000.0_umps);
}

static void test_angular_velocity() {
    std::cout << "\n== angular velocity ==\n";

    auto omega = 180.0_deg / 1s;
    EXPECT_NEAR(omega.value(), 180.0, 1e-12);

    auto omega_rad = omega.as<RadianPerSecond>();
    EXPECT_NEAR(omega_rad.value(),
                static_cast<double>(pi_ratio::num) / pi_ratio::den,
                1e-9);
    EXPECT(omega_rad ==
           1.0_radps * (static_cast<double>(pi_ratio::num) / pi_ratio::den));
    EXPECT(180.0_degps == omega_rad);

    auto ang = omega_rad * 2s;
    EXPECT_NEAR(ang.as<Degree>().value(), 360.0, 1e-6);

    auto dt = 90.0_deg / (90.0_degps);
    EXPECT_NEAR(dt.count(), 1.0, 1e-12);

    EXPECT(180_degps == 1_radps *
                            (static_cast<double>(pi_ratio::num) / pi_ratio::den));
}

static void test_mass() {
    std::cout << "\n== mass ==\n";
    auto m = 2.5_kg + Gram{500.0};
    EXPECT((std::is_same_v<decltype(m), Kilogram>));
    EXPECT_NEAR(m.value(), 3.0, 1e-12);
    EXPECT(1_kg == Gram{1000.0});
}

static void test_implicit_unit_conversion() {
    std::cout << "\n== implicit same-dimension conversion ==\n";

    Meter a{1.0};
    Kilometer d = a;
    EXPECT_NEAR(d.value(), 0.001, 1e-12);

    Centimeter cm = a;
    EXPECT_NEAR(cm.value(), 100.0, 1e-9);

    auto b = a / std::chrono::seconds{1};  // m/s
    KilometerPerHour c = b;
    EXPECT_NEAR(c.value(), 3.6, 1e-9);

    c = 10.0_mps;
    EXPECT_NEAR(c.value(), 36.0, 1e-9);

    Degree deg = 1.0_rad;
    EXPECT_NEAR(deg.value(),
                180.0 * static_cast<double>(pi_ratio::den) / pi_ratio::num,
                1e-6);
}

static void test_string_conversion() {
    std::cout << "\n== string conversion ==\n";

    EXPECT(unit_symbol<UnitMeter>() == "m");
    EXPECT(unit_symbol<UnitKilometer>() == "km");
    EXPECT(unit_symbol<UnitMillimeter>() == "mm");
    EXPECT(unit_symbol<UnitMicrometer>() == "um");
    EXPECT(unit_symbol<UnitNanometer>() == "nm");
    EXPECT(unit_symbol<UnitCentimeter>() == "cm");
    EXPECT(unit_symbol<UnitInch>() == "in");
    EXPECT(unit_symbol<UnitFoot>() == "ft");
    EXPECT(unit_symbol<UnitRadian>() == "rad");
    EXPECT(unit_symbol<UnitDegree>() == "deg");
    EXPECT(unit_symbol<UnitKilogram>() == "kg");
    EXPECT(unit_symbol<UnitGram>() == "g");

    EXPECT(unit_symbol<UnitMeterPerSecond>() == "m/s");
    EXPECT(unit_symbol<UnitMillimeterPerSecond>() == "mm/s");
    EXPECT(unit_symbol<UnitMicrometerPerSecond>() == "um/s");
    EXPECT(unit_symbol<UnitKilometerPerHour>() == "km/h");
    EXPECT(unit_symbol<UnitRadianPerSecond>() == "rad/s");
    EXPECT(unit_symbol<UnitDegreePerSecond>() == "deg/s");
    EXPECT((unit_symbol<Rate<UnitMeter, std::milli>>() == "m/ms"));
    EXPECT((unit_symbol<Rate<UnitDegree, std::ratio<60>>>() == "deg/min"));

    EXPECT(period_symbol<std::ratio<1>>() == "s");
    EXPECT(period_symbol<std::milli>() == "ms");
    EXPECT(period_symbol<std::micro>() == "us");
    EXPECT(period_symbol<std::nano>() == "ns");
    EXPECT(period_symbol<std::ratio<60>>() == "min");
    EXPECT(period_symbol<std::ratio<3600>>() == "h");
    EXPECT(period_symbol<std::ratio<86400>>() == "d");

    const auto s = to_string(3.0_m);
    EXPECT(s.find("3") != std::string::npos);
    EXPECT(s.find(" m") != std::string::npos);

    const auto vs = to_string(50.0_mps);
    EXPECT(vs.find("m/s") != std::string::npos);
}

int main() {
    test_dimensions();
    test_literals_and_aliases();
    test_length_and_same_dimension_arith();
    test_angle_and_pi_ratio();
    test_linear_velocity();
    test_angular_velocity();
    test_mass();
    test_implicit_unit_conversion();
    test_string_conversion();

    std::cout << '\n';
    if (g_failed == 0) {
        std::cout << "All tests passed.\n";
        return EXIT_SUCCESS;
    }
    std::cout << g_failed << " test(s) failed.\n";
    return EXIT_FAILURE;
}
