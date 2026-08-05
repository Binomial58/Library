# Library

競技プログラミング用の個人ライブラリです。[Competitive-Programming](https://github.com/Binomial58/Competitive-Programming) リポジトリの解答から、シンボリックリンク経由で参照しています。

## 構成

```
cpp/     自作 C++ ライブラリ
python/  Python ライブラリ（自作 + 出典明記の外部コード）
vendor/  外部ライブラリ（フォークではなくコピー）
```

### [`cpp/`](./cpp/) — 自作 C++ ライブラリ

データ構造・数学・グラフ・幾何・多項式・文字列など。各 `.hpp` に同名の `.md` で仕様・使用例を添えています。一覧は [cpp/README.md](./cpp/README.md) を参照してください。

### [`python/`](./python/) — Python ライブラリ

| ディレクトリ | 内容 | ライセンス |
|---|---|---|
| [`SortedSet/`](./python/SortedSet/) | [tatyam](https://qiita.com/tatyam/items/492c70ac4c955c055602) 氏による $O(\sqrt N)$ SortedSet/SortedMultiset/BucketList | [Unlicense](./python/SortedSet/LICENSE)（Public Domain） |

### [`vendor/`](./vendor/) — 外部ライブラリ

| ディレクトリ | 内容 | ライセンス |
|---|---|---|
| [`ac-library/`](./vendor/ac-library/) | [AtCoder Library](https://github.com/atcoder/ac-library) 公式実装 | [CC0 1.0](./vendor/ac-library/LICENSE)（Public Domain） |

## 利用方法

解答リポジトリ（Competitive-Programming）からはシンボリックリンク `library -> ../Library` 経由で参照し、必要なコードを解答ファイルへ貼り付けて使います。

```bash
# Competitive-Programming リポジトリ側
ln -s ../Library library
```

## ライセンス

- `cpp/` の自作コードには明示的なライセンスを設定していません（現状はデフォルトで著作権留保）
- `python/`・`vendor/` 配下の外部コードは、それぞれ上記表に記載のライセンスに従います
- 第三者コードを追加する際の判断基準・手順は [MAINTENANCE.md](./MAINTENANCE.md) にまとめています
