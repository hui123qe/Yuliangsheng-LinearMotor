from pathlib import Path
from zipfile import ZipFile
from lxml import etree
from openpyxl import load_workbook

ROOT = Path(__file__).resolve().parents[1]
SOURCE = Path('D:/project/上位机功能测试验收表.xlsx')
OUTPUT = ROOT / '精密直线电机可靠性测试台功能测试验收表.xlsx'

# Test descriptions are requirements for on-site verification, not passed results.
ITEMS = [
('控制-01：启动软件，验证自动读取连接配置并连接真实ACS控制器；分别检查连接成功和失败情况。',
 '验收标准：连接对象与现场控制器一致；连接成功或失败有明确提示／日志，不将模拟器结果作为真机验收结果。', False),
('控制-02：修改保护阈值和数据／截图保存位置，保存后重启软件检查恢复情况。',
 '验收标准：已保存的温度、电流、运动阈值和目录正确恢复；打开目录与界面显示一致，取消保存不误报成功。', False),
('控制-03：新建、更换测试编号，验证编号合法性及历史累计信息加载。',
 '验收标准：空编号、非法路径字符被拒绝；有效编号对应正确数据目录；更换编号后加载该编号累计次数和时长。停机状态下测试。', False),
('控制-04：分别选择0轴、1轴和双轴，执行使能、下使能并观察轴状态。',
 '验收标准：实际轴使能与选择一致；两轴状态独立显示已使能／未使能；读取失败显示状态未知，不误报已使能。', True),
('安全-01：在所选轴未使能、双轴仅一轴使能及使能状态读取失败时点击循环【运行】。',
 '验收标准：上述情况均提示并拒绝循环启动，次数不增加；单轴或双轴所需轴全部使能后允许继续启动流程。仅验收启动前使能检查。', True),
('控制-05：设置速度、加／减速度、等待时间、次数和A／B位置，检查参数下发及输入校验。',
 '验收标准：有效参数与控制器设定一致；非数字输入和超范围A／B位置被拒绝；点击【设置】时速度、加减速度按配置阈值限制。', False),
('控制-06：分别运行0轴、1轴及双轴短程循环测试，核对预定位、A／B运动、停留及目标次数。',
 '验收标准：在确认安全的−100 mm预定位及A／B范围内执行；所选轴方向、位置、速度和停留符合设置；双轴均动作，达到目标次数结束。', True),
('控制-07：检查本次／目标次数、累计次数和测试时长，停止后再次运行并重新打开软件核对。',
 '验收标准：正常A→B流程每次计数加1；时长显示天、时、分、秒；停止期间不累加，续跑及重启按已保存数据累计。软件计数不作为实际到位证明。', True),
('安全-02：循环运行中点击【停止】，检查运动停止、循环退出、计时暂停及数据保存收尾。',
 '验收标准：所选轴实际停止，不继续下一循环；次数和时长停止增加；采集完成收尾并记录保存结果，待收尾完成后可再次测试。', True),
('控制-08：在安全行程内执行正反点动、绝对运动、相对运动及手动【停止】。',
 '验收标准：轴号和方向正确；点动松开后停止；绝对／相对运动结果符合输入；停止按钮有效。测试前人工确认使能及剩余机械行程。', True),
('控制-09：操作【水冷开】【水冷关】，核对实际输出及水冷状态反馈。',
 '验收标准：开启／关闭命令与现场输出一致；两路状态一致时显示开启／关闭，不一致显示部分开启，读取失败有明确状态提示。', True),
('控制-10：操作【门锁锁定】【门锁解锁】，并分别使光幕处于正常、遮挡和读取失败状态。',
 '验收标准：门锁动作及反馈一致；光幕正常／遮挡显示正确；读取失败明确显示。绿色正常、红色异常、灰色读取失败；不将显示测试等同于互锁验收。', True),
('安全-03：释放、按下实体急停，核对硬件急停输入、电平取反配置及运行中响应。',
 '验收标准：显示与现场状态一致；触发后控制器／电气安全回路实际停止设备，上位机结束循环并进入待复位。取反配置按现场接线核对。', True),
('安全-04：模拟急停输入读取失败，再恢复正常，验证启动禁止和待复位状态。',
 '验收标准：读取失败显示灰色、禁止新启动；运行中结束上位机测试流程并锁定待复位；恢复读取后不自动续跑。不得将通信失败等同于硬件已停机。', True),
('安全-05：急停恢复后执行【复位】，分别验证未恢复、收尾未结束和正常恢复三种情况。',
 '验收标准：急停未恢复／读取失败或运动、保存未结束时拒绝复位；满足条件后执行全轴KILL及配置程序重启，提示复位完成并解除待复位。', True),
('安全-06：启用保护，在受控条件下分别验证电流、温度警戒及停机阈值。',
 '验收标准：达到警戒条件有状态／日志提示；达到停机条件时实际停机；排除异常后状态正确恢复。阈值与批准的试验条件一致，记录触发值。', True),
('采集-01：对照控制器和温度模块，核对两轴位置、速度、电流、温度及实时曲线。',
 '验收标准：轴／通道对应正确、单位正确、读数随实际变化；曲线与数值一致；模块异常和无效读数记录为问题，不作为正常测量结果。', False),
('数据-01：完成短时和较长循环测试，核对电流、温度Excel文件及停止时尾段数据。',
 '验收标准：按测试编号保存；电流含两轴通道，温度含两电机各4路；时间、设定条件及数据正确，分页连续、尾段保留、文件可打开。', False),
('数据-02：使用无效或不可写的数据目录启动采集，并检查运行中采集／保存异常提示。',
 '验收标准：采集启动失败明确提示且不进入循环；采集／保存异常写入日志，不误报保存成功；恢复有效目录后可完成一次短测试保存。', False),
('数据-03：分别保存位置、速度／加速度、温度和电流曲线截图，并检查操作、异常日志。',
 '验收标准：截图保存到设定目录且内容对应当前曲线；日志可查看运行、停止、急停、复位及保存结果，包含可追溯的时间和事件。', False),
]

assert len(ITEMS) == 20
NS = {'s': 'http://schemas.openxmlformats.org/spreadsheetml/2006/main'}
Q = '{' + NS['s'] + '}'
with ZipFile(SOURCE) as source:
    sheet = etree.fromstring(source.read('xl/worksheets/sheet1.xml'))
    workbook = etree.fromstring(source.read('xl/workbook.xml'))
    data = sheet.find('s:sheetData', NS)
    cells = {c.get('r'): c for c in data.iter(Q + 'c')}

    def write(address, value):
        c = cells[address]
        for node in list(c):
            c.remove(node)
        c.set('t', 'inlineStr')
        inline = etree.SubElement(c, Q + 'is')
        text = etree.SubElement(inline, Q + 't')
        text.text = value

    write('A1', '精密直线电机可靠性测试台功能测试验收表')
    write('A2', '范围：核心控制、安全输入、采集和数据保存，共20项。真机运动须在批准的安全条件下进行；按实测填写结果，验收通过后改为“√”。')
    for index, (item, criterion, video) in enumerate(ITEMS, 5):
        for column, value in zip('ABCDE', (item, criterion, '待验收', '待补充', '待补充' if video else '无需')):
            write(f'{column}{index}', value)
            cells[f'{column}{index}'].set('s', cells[f'{column}5'].get('s'))
    for row in list(data):
        if int(row.get('r')) > 24:
            data.remove(row)
    sheet.find('s:dimension', NS).set('ref', 'A1:E24')
    for node in workbook.findall('s:definedNames/s:definedName', NS):
        if node.get('name') == '_xlnm.Print_Area':
            node.text = "'功能验收'!$A$1:$E$24"

    replacements = {
        'xl/worksheets/sheet1.xml': etree.tostring(sheet, encoding='utf-8'),
        'xl/workbook.xml': etree.tostring(workbook, encoding='utf-8'),
    }
    with ZipFile(OUTPUT, 'w') as output:
        for entry in source.infolist():
            output.writestr(entry, replacements.get(entry.filename, source.read(entry.filename)))

original = load_workbook(SOURCE).active
result = load_workbook(OUTPUT).active
assert result.max_row == 24 and result.max_column == 5
assert result.merged_cells == original.merged_cells
assert result.page_setup == original.page_setup
assert result.page_margins == original.page_margins
for col in 'ABCDE':
    assert dict(result.column_dimensions[col]) == dict(original.column_dimensions[col])
    for r in range(5,25):
        assert result[f'{col}{r}']._style == original[f'{col}5']._style
for r in range(1,25):
    assert dict(result.row_dimensions[r]) == dict(original.row_dimensions[r])
assert all(result.cell(r,3).value=='待验收' for r in range(5,25))
with ZipFile(SOURCE) as a, ZipFile(OUTPUT) as b:
    assert a.read('xl/styles.xml') == b.read('xl/styles.xml')
    assert b.testzip() is None
print(OUTPUT)
print('20 items; original fonts, styles, dimensions, merged cells and print setup verified.')
