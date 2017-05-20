# raytracingcamp5
レイトレ合宿5用のサンプルプロジェクト

# 使い方
次の場所を自分用に更新してください。
## src
自分のレイトレを作成してください。
GCCとCV2017の両方で動くコードが良いです。
## .travis.yml
- env.global.email を自分のメアドにしてください
- env.global.username を自分の名前にしてください
- env.global.secure に暗号化したPersonal access tokenを入れてください
  - GitHubの自分のプロフィールから、Personal access token を取得
  - sudo gem install travis でtravis CI Clientをインストール
  - travis encrypt -r (ユーザー名)/(リポジトリ名) GH_TOKEN=(取得したPersonal access tokens) でPersonal access tokenを暗号化
    - 例：travis encrypt -r imagirelab/raytracingcamp5 GH_TOKEN=c45cf01c53cf64b41869b3f070e64e7783d177ed
- 最後の行のimagirelab/rtc5を公開する「ユーザー名/リポジトリ名」にしてください
## static/index.html
- 「リンク」の「src」と「出力」のアドレスを自分のサイトに修正してください。
## 公開するサイト
master ブランチをgh-pagesで公開してください。
