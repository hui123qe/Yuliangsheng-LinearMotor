from copy import deepcopy
from pathlib import Path
from zipfile import ZipFile
from io import BytesIO
from docx import Document
from docx.text.paragraph import Paragraph
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

ROOT = Path(__file__).resolve().parents[1]
SOURCE = Path('D:/project/测试台软件操作手册.docx')
OUTPUT = ROOT / '精密直线电机可靠性测试台软件操作手册（模板精简版）.docx'
doc = Document(SOURCE)
blocks = list(doc.element.body)
tables = list(doc.tables)

def replace(p, text):
    # Keep the original paragraph and run formatting, including cover fonts.
    runs = p.runs
    template = deepcopy(runs[0]._r.rPr) if runs and runs[0]._r.rPr is not None else None
    for child in list(p._p):
        if child.tag not in (qn('w:pPr'), qn('w:bookmarkStart'), qn('w:bookmarkEnd')):
            p._p.remove(child)
    r = p.add_run(text)
    if template is not None:
        r._r.insert(0, template)

def paragraph(index, text):
    replace(Paragraph(blocks[index], doc), text)

def cell(c, text):
    lines = text.split('\n')
    ps = list(c.paragraphs)
    for i, line in enumerate(lines):
        if i >= len(ps):
            node = deepcopy(ps[-1]._p)
            c._tc.append(node)
            ps.append(Paragraph(node, c))
        replace(ps[i], line)
    for p in ps[len(lines):]:
        c._tc.remove(p._p)

def table(index, rows):
    t = tables[index]
    for i, row in enumerate(rows):
        for j, text in enumerate(row):
            cell(t.rows[i].cells[j], text)
    for r in list(t.rows)[len(rows):]:
        t._tbl.remove(r._tr)

def picture_table(index, title, figures):
    table(index, [(title, title)] + [
        ('【此处插入截图】\n' + name + '\n截图标注：' + labels + '\n图' + number,
         description) for name, labels, number, description in figures])

texts = {
5:'精密直线电机可靠性测试台',
14:'适用软件：Linear_motor_905（当前交付版本）',
15:'编写日期：2026年11月6日',
44:'本软件用于精密直线电机可靠性测试台的0轴、1轴及双轴运动控制，支持循环测试、温度与电流监测、数据保存和日志查看。',
46:'使用随设备交付的 Windows 64位工控机和完整软件目录，保留配套 DLL、插件及配置文件。',
47:'确认 ACS 控制器、温度模块及现场水冷、安全装置已完成配置。',
50:'① 将交付目录完整放置在工控机上。\n② 检查设备供电、网线和数据保存目录。',
51:'2.2 软件启动',
52:'① 双击 Linear_motor_905.exe，软件自动尝试连接控制器。\n② 查看日志与状态反馈，确认现场使用真实控制器。',
53:'控制器连接失败或状态读取失败时，先检查供电和通信；恢复后再启动测试。',
61:'3.2 状态与辅助控制',
62:'顶部提供【水冷开／关】【门锁锁定／解锁】及门锁、光幕、急停状态。绿色为正常，红色为异常，灰色为读取中／失败，橙色为急停恢复后待复位。',
65:'3.3 手动调试',
66:'手动调试区提供点动、绝对运动、相对运动和停止。操作前核对轴号、使能和机械行程。',
69:'4.1 测试准备',
72:'确认运动区域无人、试件固定可靠，水冷与现场安全装置满足测试条件。',
74:'4.2 循环运行测试',
77:'启动前检查所选轴使能；双轴要求两轴均已使能。未使能或状态读取失败时拒绝启动。',
79:'4.2 循环运行测试（续）',
82:'循环次数为软件流程计数，须结合实际位置和运动状态确认。结束后等待数据保存完成再退出软件。',
85:'手动操作前先结束循环测试，确认轴号和使能状态，使用经现场确认的速度与行程。',
88:'紧急情况使用实体急停；软件【急停】向控制器发送全轴停止命令，依赖通信。',
91:'测试运行参数在测试区输入并点击【设置】；保护阈值在设置页修改后点击【保存设置】。修改前先停止测试。',
92:'5.1 核心测试参数',
93:'参数按批准的试验要求填写，不直接使用界面示例值作为试验条件。',
95:'行程A／B必须在最小、最大行程范围内；启动前还应确认预定位位置 −100 mm 可安全到达。',
99:'5.2 保存位置与保护设置',
100:'设置页可修改数据、截图保存位置及保护阈值，使用【打开目录】核对保存位置。',
107:'出现异常先停止操作，查看状态、弹窗及【日志】，按原因排查。',
110:'温度、电流等异常应核实现场读数和保护设置；读取失败不能当作正常。',
112:'6.2 急停恢复与复位',
115:'复位会执行全轴 KILL 并重启配置的控制程序。确认设备停稳、急停恢复且数据保存结束后再操作。',
117:'安全操作提示',
118:'设备由受训人员操作。运动前确认试件固定、行程与安全装置；异常时立即停止并联系维护人员。',
122:'本手册按当前软件核心功能编写，现场以实际交付版本及设备操作规程为准。',
}
for i, text in texts.items():
    paragraph(i,text)

table(0,[('项目','内容'),('适用程序','Linear_motor_905（当前交付版本）'),('硬件兼容','本项目 ACS 控制器、双轴测试台及配套传感器'),('编写日期','2026/11/06'),('适用软件','精密直线电机可靠性测试台')])
table(1,[('修订日期','版本','操作'),('2026/11/06','V1.0','沿用原模板，精简核心操作内容')])
picture_table(2,'主界面区域',[
('软件主界面全图','①顶部状态与控制；②统计信息；③实时曲线；④测试操作区','3-1',
 '主界面核心功能\n① 顶部：水冷、门锁、光幕、急停、复位与日志。\n② 统计：测试时长、循环次数、温度、电流及位置。\n③ 曲线：查看实时数据，支持【截图】。\n④ 测试区：编号、轴号、参数、使能与运行。')])
table(3,[('区域','功能'),('统计信息','查看测试时长和累计循环次数。'),('实时曲线','查看位置、速度／加速度、温度和电流。'),('测试操作','测试编号、轴号、参数设置、使能、运行和停止。'),('轴状态','显示0轴、1轴已使能／未使能／状态未知。')])
picture_table(4,'状态与辅助控制',[
('顶部状态及控制区域','①水冷；②门锁；③光幕；④急停与复位','3-2',
 '状态确认\n① 按现场要求开启水冷、锁定门锁。\n② 核对门锁、光幕与急停状态。\n③ 红色或灰色时先排查，不开始测试。\n④ 急停恢复后显示待复位，按6.2节处理。')])
picture_table(5,'测试编号与轴使能',[
('测试区编号、轴号与使能','①新建／更换；②轴号；③使能按钮；④两轴状态','4-1',
 '测试准备\n① 停机后点击【新建／更换】，输入测试编号。\n② 选择0轴、1轴或双轴。\n③ 点击【使能】，观察所选轴状态。\n④ 双轴运行前确认两轴均已使能。')])
picture_table(6,'参数设置与循环运行',[
('循环运动参数区','①速度；②加／减速度；③等待时间；④次数；⑤行程A／B','4-2',
 '（1）设置参数\n① 填写速度、加／减速度、等待时间和循环次数。\n② 核对A／B目标位置及机械行程。\n③ 点击【设置】，复核显示值。'),
('循环运动按钮与状态','①运行；②停止；③使能状态；④次数／时长','4-3',
 '（2）运行与观察\n① 确认安全和使能后点击【运行】。\n② 程序先定位至 −100 mm，再执行A点→等待→B点→等待。\n③ 观察位置、温度、电流和循环次数。\n④ 需要中止时点击【停止】。')])
picture_table(7,'数据保存与截图',[
('保存位置设置与曲线截图按钮','①数据目录；②截图目录；③打开目录；④截图','4-4',
 '查看测试记录\n① 循环运行时自动采集，结束后等待保存完成。\n② 在“数据目录／data／测试编号”内查看电流、温度Excel文件。\n③ 点击曲线【截图】保存当前图像。\n④ 通过【日志】查看保存结果及异常。')])
picture_table(8,'手动调试操作',[
('手动运动控制区','①点动＋／－；②绝对运动；③相对运动；④停止','4-5',
 '手动定位\n① 核对轴号、使能、当前位置和剩余行程。\n② 点动：按住运动，松开停止。\n③ 绝对运动输入目标位置；相对运动输入位移。\n④ 完成后确认停稳，再下使能。')])
table(9,[('参数','单位','说明'),('测试编号','—','用于区分测试及数据目录；停机后更换。'),('轴号','—','0轴、1轴或双轴。'),('速度','mm/s','按试验要求设置，受速度阈值限制。'),('加／减速度','mm/s²','按试验要求设置，受加减速度阈值限制。'),('等待时间','ms','A、B点各自的停留时间。'),('循环次数','次','A点→B点为一次软件循环；显示已完成／目标。'),('行程A／B','mm','往复运动目标位置。')])
table(11,[('配置内容','操作说明'),('保护阈值','按设备要求设置温度、电流、流量、气压及运动阈值，点击【保存设置】。'),('数据保存','设置数据目录；测试按编号保存电流和温度Excel文件。'),('截图保存','设置截图目录，通过曲线页【截图】保存图像。'),('控制器通信','由维护人员核对程序目录中的连接配置；现场测试必须使用真实控制器。')])
picture_table(12,'保存参数配置',[
('设置页保护阈值及保存按钮','①警戒值；②运动阈值；③开启保护；④保存设置','5-1',
 '参数维护\n① 停止测试后核对保护与运动阈值。\n② 按现场要求启用保护。\n③ 点击【保存设置】并确认，等待保存成功提示。')])
table(13,[('提示／现象','可能原因','处理方法'),('控制器连接失败','供电、网络或连接配置异常。','检查供电与网线，由维护人员核对配置。'),('所选轴未使能','未使能或状态读取失败。','确认轴号并使能；双轴需两轴均使能。'),('急停触发／读取失败','急停动作或通信异常。','排除原因并恢复读取，按6.2节复位。'),('目标超出行程','A／B位置超出设置范围。','核对目标和机械行程，修正参数。'),('采集／保存失败','采集未就绪、路径不可写等。','查看日志，检查目录、磁盘和采集设备。'),('温度／电流异常','读数异常或达到保护阈值。','停止检查散热、负载及传感器，排除原因后运行。')])
picture_table(14,'急停恢复与复位',[
('顶部急停状态、复位按钮及日志','①急停状态；②复位；③日志；④轴使能','6-1',
 '恢复操作\n① 确认停稳，排除原因并释放实体急停。\n② 急停显示“正常，待复位”后点击【复位】。\n③ 等待日志提示“复位完成”。\n④ 核对轴状态，重新使能后再开始测试。')])

# Drop redundant content, keeping the source page and paragraph formatting.
for i in [96,97,98,102,104,105,119,120,121]:
    el=blocks[i]
    el.getparent().remove(el)

# Keep the template TOC line styles, but replace stale page numbers with page fields.
headings=[p for p in doc.paragraphs if p.style.name.startswith('Heading')]
toc=list(range(18,37))
toc_nodes=[blocks[i] for i in toc]
while len(toc_nodes)<len(headings):
    node=deepcopy(toc_nodes[-1]);toc_nodes[-1].addnext(node);toc_nodes.append(node)
for node,p in zip(toc_nodes,headings):
    target=Paragraph(node,doc)
    replace(target,p.text+'\t')
    bookmarks=p._p.findall(qn('w:bookmarkStart'))
    if bookmarks:
        field=OxmlElement('w:fldSimple')
        field.set(qn('w:instr'),' PAGEREF '+bookmarks[0].get(qn('w:name'))+' \\h ')
        field.set(qn('w:dirty'),'true')
        r=OxmlElement('w:r'); t=OxmlElement('w:t');t.text=''; r.append(t);field.append(r)
        target._p.append(field)
for idx in toc[len(headings):]:
    el=blocks[idx];el.getparent().remove(el)
update=doc.settings.element.find(qn('w:updateFields'))
if update is None:
    update=OxmlElement('w:updateFields');doc.settings.element.append(update)
update.set(qn('w:val'),'true')
doc.core_properties.title='精密直线电机可靠性测试台软件操作手册'
generated=BytesIO()
doc.save(generated)
# Copy unchanged package parts verbatim; python-docx otherwise reserializes them.
with ZipFile(SOURCE) as original, ZipFile(generated) as changed, ZipFile(OUTPUT,'w') as result:
    for item in original.infolist():
        data=(changed.read(item.filename) if item.filename in
              ('word/document.xml','word/settings.xml','docProps/core.xml')
              else original.read(item.filename))
        result.writestr(item,data)

with ZipFile(SOURCE) as a,ZipFile(OUTPUT) as b:
    assert a.read('word/styles.xml') == b.read('word/styles.xml'), 'Template styles changed'
    assert a.read('word/fontTable.xml') == b.read('word/fontTable.xml'), 'Template fonts changed'
    assert b.testzip() is None
check=Document(OUTPUT)
assert check.sections[0]._sectPr.xml == Document(SOURCE).sections[0]._sectPr.xml
assert len(check.tables)==14
print(OUTPUT)
print('Template styles, font table and page setup preserved. Tables:',len(check.tables))
