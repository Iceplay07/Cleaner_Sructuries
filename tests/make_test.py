from pathlib import Path
import shutil
import argparse


ROOT = Path(__file__).resolve().parent


def reset_dir(path: Path):
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True)


def write_lines(path: Path, lines):
    with open(path, "w", encoding="utf-8") as f:
        for line in lines:
            f.write(line)


def write_cpp_file(path: Path, size_mb: int, prefix: str):
    target_size = size_mb * 1024 * 1024
    written = 0
    i = 0

    with open(path, "w", encoding="utf-8") as f:
        while written < target_size:
            line = f"int {prefix}_{i:09d} = {i}; // generated line {i}\n"
            f.write(line)
            written += len(line.encode("utf-8"))
            i += 1


def write_similar_to_base(path: Path, size_mb: int):
    target_size = size_mb * 1024 * 1024
    written = 0
    i = 0

    with open(path, "w", encoding="utf-8") as f:
        while written < target_size:
            if i % 10 == 0:
                line = f"int changed_{i:09d} = {i}; // changed line {i}\n"
            else:
                line = f"int base_{i:09d} = {i}; // generated line {i}\n"

            f.write(line)
            written += len(line.encode("utf-8"))
            i += 1


# test1: маленький тест на дубликаты
def make_test1():
    path = ROOT / "test1"
    reset_dir(path)

    text = [
        "#include <iostream>\n",
        "int main() {\n",
        "    std::cout << 123 << std::endl;\n",
        "    return 0;\n",
        "}\n",
    ]

    write_lines(path / "a.cpp", text)
    write_lines(path / "b.cpp", text)
    write_lines(path / "c.cpp", [
        "#include <iostream>\n",
        "int main() {\n",
        "    std::cout << 777 << std::endl;\n",
        "    return 0;\n",
        "}\n",
    ])


# test2: маленький тест на похожесть
def make_test2():
    path = ROOT / "test2"
    reset_dir(path)

    base = [
        "int a0 = 0;\n",
        "int a1 = 1;\n",
        "int a2 = 2;\n",
        "int a3 = 3;\n",
        "int a4 = 4;\n",
        "int a5 = 5;\n",
        "int a6 = 6;\n",
        "int a7 = 7;\n",
        "int a8 = 8;\n",
        "int a9 = 9;\n",
    ]

    similar = [
        "int a0 = 0;\n",
        "int a1 = 1;\n",
        "int a2 = 2;\n",
        "int a3 = 3;\n",
        "int a4 = 4;\n",
        "int a5 = 5;\n",
        "int a6 = 6;\n",
        "int a7 = 7;\n",
        "int changed1 = 100;\n",
        "int changed2 = 200;\n",
    ]

    write_lines(path / "base.cpp", base)
    write_lines(path / "similar.cpp", similar)
    write_lines(path / "different.cpp", [
        "double x = 1.5;\n",
        "double y = 2.5;\n",
        "double z = x + y;\n",
    ])


# test3: пустые файлы и игнорируемые расширения
def make_test3():
    path = ROOT / "test3"
    reset_dir(path)

    write_lines(path / "empty.cpp", [])
    write_lines(path / "spaces.cpp", ["   \n", "\t\n", "       \n"])
    write_lines(path / "normal.cpp", ["int main() { return 0; }\n"])
    write_lines(path / "image.png", ["fake image data\n"])
    write_lines(path / "archive.zip", ["fake archive data\n"])


# test4: много маленьких файлов
def make_test4(count: int):
    path = ROOT / "test4"
    reset_dir(path)

    for i in range(count):
        write_lines(path / f"file_{i:05d}.cpp", [
            f"int value_{i} = {i};\n",
            f"int square_{i} = {i} * {i};\n",
        ])


# test5: большие точные дубликаты
def make_test5(size_mb: int):
    path = ROOT / "test5"
    reset_dir(path)

    write_cpp_file(path / "large_a.cpp", size_mb, "dup")
    shutil.copyfile(path / "large_a.cpp", path / "large_b.cpp")


# test6: большие похожие файлы примерно на 90%
def make_test6(size_mb: int):
    path = ROOT / "test6"
    reset_dir(path)

    write_cpp_file(path / "base.cpp", size_mb, "base")
    write_similar_to_base(path / "similar_90.cpp", size_mb)


# test7: много средних уникальных файлов
def make_test7(count: int, size_mb: int):
    path = ROOT / "test7"
    reset_dir(path)

    for i in range(count):
        write_cpp_file(path / f"unique_{i:03d}.cpp", size_mb, f"unique_{i}")


# test8: группы дубликатов
def make_test8(size_mb: int):
    path = ROOT / "test8"
    reset_dir(path)

    for group in range(5):
        original = path / f"group_{group}_a.cpp"

        write_cpp_file(original, size_mb, f"group_{group}")

        shutil.copyfile(original, path / f"group_{group}_b.cpp")
        shutil.copyfile(original, path / f"group_{group}_c.cpp")


# test9: много пустых файлов
def make_test9(count: int):
    path = ROOT / "test9"
    reset_dir(path)

    for i in range(count):
        write_lines(path / f"empty_{i:05d}.cpp", [])

    for i in range(20):
        write_lines(path / f"normal_{i:02d}.cpp", [
            f"int normal_{i} = {i};\n"
        ])


# test10: .hpp и .cpp с одинаковым содержимым
def make_test10(size_mb: int):
    path = ROOT / "test10"
    reset_dir(path)

    write_cpp_file(path / "same_content.hpp", size_mb, "same")
    shutil.copyfile(path / "same_content.hpp", path / "same_content.cpp")


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--big-mb",
        type=int,
        default=50,
        help="Размер больших файлов в MB"
    )

    parser.add_argument(
        "--small-count",
        type=int,
        default=3000,
        help="Количество маленьких файлов для test4"
    )

    parser.add_argument(
        "--empty-count",
        type=int,
        default=10000,
        help="Количество пустых файлов для test9"
    )

    parser.add_argument(
        "--medium-count",
        type=int,
        default=50,
        help="Количество средних файлов для test7"
    )

    args = parser.parse_args()

    make_test1()
    make_test2()
    make_test3()

    make_test4(args.small_count)
    make_test5(args.big_mb)
    make_test6(args.big_mb)
    make_test7(args.medium_count, max(1, args.big_mb // 10))
    make_test8(max(1, args.big_mb // 5))
    make_test9(args.empty_count)
    make_test10(args.big_mb)

    print("Created test1 - test10 in:", ROOT)


if __name__ == "__main__":
    main()