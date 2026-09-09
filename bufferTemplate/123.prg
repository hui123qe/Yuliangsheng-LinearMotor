#/ Controller version = 4.20
#/ Date = 9/9/2026 8:08 AM
#/ User remarks = 
#0
!PNAME=IO Mapping 
!PDESC=
AUTOEXEC:

while 1
! 数字量输出
ECOUT(134,EC_DOUT1)
ECOUT(135,EC_DOUT2)
EC_DOUT1.0=Value_OUT1! 水冷进水电磁阀
EC_DOUT1.1=Value_OUT2! 水冷水电磁阀
EC_DOUT1.2=Red_OUT! 三色灯红输出
EC_DOUT1.3=Yellow_OUT! 三色灯黄输出
EC_DOUT1.4=Green_OUT! 三色灯绿输出
EC_DOUT1.5=Buzzer_OUT! 蜂鸣器输出
EC_DOUT1.6=NC_DOUT1! 预留
EC_DOUT1.7=NC_DOUT2! 预留
EC_DOUT2.0=NC_DOUT3! 预留
EC_DOUT2.1=NC_DOUT4! 预留
EC_DOUT2.2=NC_DOUT5! 预留
EC_DOUT2.3=NC_DOUT6! 预留
EC_DOUT2.4=NC_DOUT7! 预留
EC_DOUT2.5=NC_DOUT8! 预留
EC_DOUT2.6=NC_DOUT9! 预留
EC_DOUT2.7=NC_DOUT10! 预留

! 数字量输入
ECIN(180,EC_DIN1)
ECIN(181,EC_DIN2)
PRV_IN=EC_DIN1.0! 总进气压力开关
X1_LimitP=EC_DIN1.1! X1正限位
X1_LimitN=EC_DIN1.2! X1负限位
X2_LimitP=EC_DIN1.4! X2正限位
X2_LimitN=EC_DIN1.3! X2负限位
NC_DIN1=EC_DIN1.5! 预留
NC_DIN2=EC_DIN1.6! 预留
NC_DIN3=EC_DIN1.7! 预留
NC_DIN4=EC_DIN2.0! 预留
NC_DIN5=EC_DIN2.1! 预留
NC_DIN6=EC_DIN2.2! 预留
NC_DIN7=EC_DIN2.3! 预留
NC_DIN8=EC_DIN2.4! 预留
NC_DIN9=EC_DIN2.5! 预留
NC_DIN10=EC_DIN2.6! 预留
NC_DIN11=EC_DIN2.7! 预留

!模拟量输入
ECIN(162,EC_AIN1)! 总进气流量传感器，流量传感器3，3008第4输入口
ECIN(178,EC_AIN2)! 总进气压力传感器，压力传感器5，3008第8输入口
ECIN(150,EC_AIN3)! X1水冷流量传感器，流量传感器1，3008第1输入口
ECIN(174,EC_AIN4)! X1水冷进水压力传感器，压力传感器2，3008第7输入口
ECIN(166,EC_AIN5)! X1水冷出水压力传感器，压力传感器1，3008第5输入口
ECIN(158,EC_AIN6)! X2水冷流量传感器，流量传感器2，3008第3输入口
ECIN(170,EC_AIN7)! X2水冷进水压力传感器，压力传感器3，3008第6输入口
ECIN(154,EC_AIN8)! X2水冷出水压力传感器，压力传感器4，3008第2输入口

Main_Flow=EC_AIN1/32767*600! 总进气流量值，单位L/min
Main_Pressure=(EC_AIN2/32767*10-1)*0.25! 总进气压力值，单位MPa
X1_Flow=(EC_AIN3/32767*10-1)*10! X1水冷流量值，单位L/min
X1_Pressure1=(EC_AIN4/32767*10-1)*0.25! X1水冷进水压力值，单位MPa
X1_Pressure2=(EC_AIN5/32767*10-1)*0.25! X1水冷出水压力值，单位MPa
X2_Flow=(EC_AIN6/32767*10-1)*10! X2水冷流量值，单位L/min
X2_Pressure1=(EC_AIN7/32767*10-1)*0.25! X2水冷进水压力值，单位MPa
X2_Pressure2=(EC_AIN8/32767*10-1)*0.25! X2水冷出水压力值，单位MPa

CurrentX1=COEREAD/2(0,0X6077,0)/1000*61.509
CurrentX2=COEREAD/2(1,0X6077,0)/1000*61.509

end
STOP
#1
!PNAME=Limit
!PDESC=
AUTOEXEC:

while 1
IF X1_LimitP=0 | X1_LimitN=0 
KILL 0
end

IF X2_LimitP=0 | X2_LimitN=0
KILL 1
end
end

STOP

#2
!PNAME=dc
!PDESC=

!PNAME=Standard Acquisition Service
!PDESC=Persistent circular block data acquisition service


!============================================================
! Standard Acquisition Service
!
! Buffer should be started once and remain alive.
!
!
! Public command:
!
!     ACQ_RUN = 1    Start new acquisition session
!
!     ACQ_RUN = 0    Stop acquisition
!
!
! Public notification:
!
!     ACQ_PUBLISHED_SEQ
!
! This buffer only performs data acquisition.
!============================================================


!============================================================
! Internal acquisition state
!============================================================

INT ACQ_SESSION_ID 
INT ACQ_CURRENT_BLOCK 
INT ACQ_CURRENT_VALID_COUNT
INT ACQ_STOP_REQUEST 
INT ACQ_BLOCK_SEQUENCE 

INT ACQ_BLOCK_COUNT =5
INT ACQ_SAMPLE_PERIOD =1
INT ACQ_BLOCK_SIZE =16666

!============================================================
! Service initialization
!============================================================

ACQ_RUN = 0

ACQ_STATE = 0

ACQ_CURRENT_BLOCK = 0

ACQ_CURRENT_VALID_COUNT = 0

ACQ_STOP_REQUEST = 0

ACQ_BLOCK_SEQUENCE = 0


ACQ_PUBLISHED_SESSION = 0
ACQ_PUBLISHED_BLOCK = 0
ACQ_PUBLISHED_COUNT = 0
ACQ_PUBLISHED_PARTIAL = 0
ACQ_PUBLISHED_SEQ = 0


!------------------------------------------------------------
! Clear initial block metadata
!------------------------------------------------------------

ACQ_BLOCK_VALID_COUNT(0) = 0
ACQ_BLOCK_VALID_COUNT(1) = 0
ACQ_BLOCK_VALID_COUNT(2) = 0
ACQ_BLOCK_VALID_COUNT(3) = 0
ACQ_BLOCK_VALID_COUNT(4) = 0


ACQ_BLOCK_PARTIAL(0) = 0
ACQ_BLOCK_PARTIAL(1) = 0
ACQ_BLOCK_PARTIAL(2) = 0
ACQ_BLOCK_PARTIAL(3) = 0
ACQ_BLOCK_PARTIAL(4) = 0


ACQ_BLOCK_SEQ(0) = 0
ACQ_BLOCK_SEQ(1) = 0
ACQ_BLOCK_SEQ(2) = 0
ACQ_BLOCK_SEQ(3) = 0
ACQ_BLOCK_SEQ(4) = 0


ACQ_BLOCK_SESSION(0) = 0
ACQ_BLOCK_SESSION(1) = 0
ACQ_BLOCK_SESSION(2) = 0
ACQ_BLOCK_SESSION(3) = 0
ACQ_BLOCK_SESSION(4) = 0



!============================================================
! Persistent acquisition service
!============================================================

WHILE 1


    TILL ACQ_RUN = 1

    !========================================================
    ! Start new acquisition session
    !========================================================

    ACQ_STATE = 1


    !--------------------------------------------------------
    ! Generate new session ID
    !--------------------------------------------------------

    ACQ_SESSION_ID = ACQ_SESSION_ID + 1


    !--------------------------------------------------------
    ! Reset current session runtime
    !--------------------------------------------------------

    ACQ_CURRENT_BLOCK = 0

    ACQ_CURRENT_VALID_COUNT = 0

    ACQ_STOP_REQUEST = 0

    ACQ_BLOCK_SEQUENCE = 0


    !--------------------------------------------------------
    ! Reset latest publication
    !
    ! Important:
    !
    ! ACQ_PUBLISHED_SEQ = 0 means:
    !
    !     no block has been published in this session yet.
    !--------------------------------------------------------

    ACQ_PUBLISHED_SESSION = ACQ_SESSION_ID

    ACQ_PUBLISHED_BLOCK = 0

    ACQ_PUBLISHED_COUNT = 0

    ACQ_PUBLISHED_PARTIAL = 0

    ACQ_PUBLISHED_SEQ = 0

    !--------------------------------------------------------
    ! Invalidate all circular blocks
    !--------------------------------------------------------

    ACQ_BLOCK_VALID_COUNT(0) = 0
    ACQ_BLOCK_VALID_COUNT(1) = 0
    ACQ_BLOCK_VALID_COUNT(2) = 0
    ACQ_BLOCK_VALID_COUNT(3) = 0
    ACQ_BLOCK_VALID_COUNT(4) = 0


    ACQ_BLOCK_PARTIAL(0) = 0
    ACQ_BLOCK_PARTIAL(1) = 0
    ACQ_BLOCK_PARTIAL(2) = 0
    ACQ_BLOCK_PARTIAL(3) = 0
    ACQ_BLOCK_PARTIAL(4) = 0


    ACQ_BLOCK_SEQ(0) = 0
    ACQ_BLOCK_SEQ(1) = 0
    ACQ_BLOCK_SEQ(2) = 0
    ACQ_BLOCK_SEQ(3) = 0
    ACQ_BLOCK_SEQ(4) = 0


    ACQ_BLOCK_SESSION(0) = 0
    ACQ_BLOCK_SESSION(1) = 0
    ACQ_BLOCK_SESSION(2) = 0
    ACQ_BLOCK_SESSION(3) = 0
    ACQ_BLOCK_SESSION(4) = 0


    !--------------------------------------------------------
    ! Acquisition is now running
    !--------------------------------------------------------

    ACQ_STATE = 2


    !========================================================
    ! Continuous circular acquisition
    !========================================================

    WHILE ACQ_RUN & ^ACQ_STOP_REQUEST


        !====================================================
        ! Select next physical block
        !====================================================

        ACQ_CURRENT_BLOCK = ACQ_CURRENT_BLOCK + 1


        IF ACQ_CURRENT_BLOCK > ACQ_BLOCK_COUNT

            ACQ_CURRENT_BLOCK = 1

        END



        !====================================================
        ! Mark this block as being reused
        !
        ! IMPORTANT:
        !
        ! Sequence is cleared FIRST.
        !
        ! The host must treat:
        !
        !     ACQ_BLOCK_SEQ(index) = 0
        !
        ! as invalid / being written.
        !====================================================

        ACQ_BLOCK_SEQ(ACQ_CURRENT_BLOCK - 1) = 0

        ACQ_BLOCK_SESSION(ACQ_CURRENT_BLOCK - 1) = 0

        ACQ_BLOCK_VALID_COUNT(ACQ_CURRENT_BLOCK - 1) = 0

        ACQ_BLOCK_PARTIAL(ACQ_CURRENT_BLOCK - 1) = 0



        !====================================================
        ! Start physical DC acquisition
        !
        ! Acquisition Profile:        !
        !
        !====================================================

        IF ACQ_CURRENT_BLOCK = 1

            DC ACQ_DATA_1, ACQ_BLOCK_SIZE, ACQ_SAMPLE_PERIOD, CurrentX1, CurrentX2


        ELSEIF ACQ_CURRENT_BLOCK = 2

            DC ACQ_DATA_2, ACQ_BLOCK_SIZE, ACQ_SAMPLE_PERIOD, CurrentX1, CurrentX2


        ELSEIF ACQ_CURRENT_BLOCK = 3

            DC ACQ_DATA_3, ACQ_BLOCK_SIZE, ACQ_SAMPLE_PERIOD, CurrentX1, CurrentX2


        ELSEIF ACQ_CURRENT_BLOCK = 4

            DC ACQ_DATA_4, ACQ_BLOCK_SIZE, ACQ_SAMPLE_PERIOD, CurrentX1, CurrentX2


        ELSEIF ACQ_CURRENT_BLOCK = 5

            DC ACQ_DATA_5, ACQ_BLOCK_SIZE, ACQ_SAMPLE_PERIOD, CurrentX1, CurrentX2


        END



        !====================================================
        ! Wait for:
        !
        ! A. normal DC completion
        !
        ! OR
        !
        ! B. STOPDC caused by ACQ_RUN = 0
        !====================================================

        TILL ^S_ST.#DC


        !====================================================
        ! Capture S_DCN immediately
        !
        ! Normal block:
        !
        !     S_DCN = ACQ_BLOCK_SIZE
        !
        ! STOPDC:
        !
        !     0 <= S_DCN < ACQ_BLOCK_SIZE
        !====================================================

        ACQ_CURRENT_VALID_COUNT = S_DCN

        !----------------------------------------------------
        ! Defensive range check
        !----------------------------------------------------

        IF ACQ_CURRENT_VALID_COUNT < 0

            ACQ_CURRENT_VALID_COUNT = 0

        END


        IF ACQ_CURRENT_VALID_COUNT > ACQ_BLOCK_SIZE

            ACQ_CURRENT_VALID_COUNT = ACQ_BLOCK_SIZE

        END



        !====================================================
        ! Publish block only if it contains data
        !====================================================

        IF ACQ_CURRENT_VALID_COUNT > 0


            !================================================
            ! Step 1
            !
            ! Write block metadata except sequence.
            !================================================

            ACQ_BLOCK_VALID_COUNT(ACQ_CURRENT_BLOCK - 1) = ACQ_CURRENT_VALID_COUNT


            !================================================
            ! Step 2
            !
            ! Determine whether this is a partial block.
            !================================================

            IF ACQ_CURRENT_VALID_COUNT < ACQ_BLOCK_SIZE

                ACQ_BLOCK_PARTIAL(ACQ_CURRENT_BLOCK - 1) = 1

            ELSE

                ACQ_BLOCK_PARTIAL(ACQ_CURRENT_BLOCK - 1) = 0

            END



            !================================================
            ! Step 3
            !
            ! Assign session.
            !================================================

            ACQ_BLOCK_SESSION(ACQ_CURRENT_BLOCK - 1) = ACQ_SESSION_ID



            !================================================
            ! Step 4
            !
            ! Generate new sequence number.
            !================================================

            ACQ_BLOCK_SEQUENCE = ACQ_BLOCK_SEQUENCE + 1



            !================================================
            ! Step 5
            !
            ! BLOCK COMMIT
            !
            ! Sequence is written LAST into per-block metadata.
            !
            ! Once ACQ_BLOCK_SEQ != 0,
            ! the block is considered valid.
            !================================================

            ACQ_BLOCK_SEQ(ACQ_CURRENT_BLOCK - 1) = ACQ_BLOCK_SEQUENCE



            !================================================
            ! Step 6
            !
            ! Update latest publication information.
            !
            ! ACQ_PUBLISHED_SEQ must be written LAST.
            !================================================

            ACQ_PUBLISHED_SESSION = ACQ_SESSION_ID

            ACQ_PUBLISHED_BLOCK = ACQ_CURRENT_BLOCK

            ACQ_PUBLISHED_COUNT = ACQ_CURRENT_VALID_COUNT

            ACQ_PUBLISHED_PARTIAL = ACQ_BLOCK_PARTIAL(ACQ_CURRENT_BLOCK - 1)



            !================================================
            ! GLOBAL PUBLICATION COMMIT
            !================================================

            ACQ_PUBLISHED_SEQ = ACQ_BLOCK_SEQUENCE


        END



    END



    !========================================================
    ! Stop current acquisition session
    !========================================================

    ACQ_STATE = 3

    !--------------------------------------------------------
    ! Return to idle
    !--------------------------------------------------------

    ACQ_STATE = 0



END



STOP



!============================================================
! ACQ_RUN change handler
!
! Falling edge:
!
!     ACQ_RUN:
!
!         1 -> 0
!
! immediately terminates an active DC collection.
!
! Main service will then:
!
!     TILL ^S_ST.#DC
!             ↓
!          S_DCN
!             ↓
!       publish partial block
!
!============================================================

ON ^ACQ_RUN

BLOCK


    IF ACQ_RUN = 0


        !----------------------------------------------------
        ! Latch stop.
        !
        ! Even if the host changes ACQ_RUN back to 1 very
        ! quickly, current session must terminate first.
        !----------------------------------------------------

        ACQ_STOP_REQUEST = 1



        !----------------------------------------------------
        ! Stop active system DC immediately.
        !----------------------------------------------------

        IF S_ST.#DC

            STOPDC

        END


    END


END


RET

#A
!PNAME=
!PDESC=
!axisdef X=0,Y=1,Z=2,T=3,A=4,B=5,C=6,D=7
!axisdef x=0,y=1,z=2,t=3,a=4,b=5,c=6,d=7
! global int I(100),I0,I1,I2,I3,I4,I5,I6,I7,I8,I9,I90,I91,I92,I93,I94,I95,I96,I97,I98,I99
! global real V(100),V0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V90,V91,V92,V93,V94,V95,V96,V97,V98,V99
global int EC_DOUT1,EC_DOUT2,EC_DIN1,EC_DIN2
global real EC_AIN1,EC_AIN2,EC_AIN3,EC_AIN4,EC_AIN5,EC_AIN6,EC_AIN7,EC_AIN8
global int Value_OUT1,Value_OUT2,Red_OUT,Yellow_OUT,Green_OUT,Buzzer_OUT,NC_DOUT1,NC_DOUT2,NC_DOUT3,NC_DOUT4,NC_DOUT5,NC_DOUT6,NC_DOUT7,NC_DOUT8,NC_DOUT9,NC_DOUT10! 数字量输出变量定义
global int PRV_IN,X1_LimitP,X1_LimitN,X2_LimitN,X2_LimitP,NC_DIN1,NC_DIN2,NC_DIN3,NC_DIN4,NC_DIN5,NC_DIN6,NC_DIN7,NC_DIN8,NC_DIN9,NC_DIN10,NC_DIN11! 数字量输入变量定义
global real Main_Flow,Main_Pressure,X1_Flow,X1_Pressure1,X1_Pressure2,X2_Flow,X2_Pressure1,X2_Pressure2! 模拟量输入变量定义
global real CurrentX1,CurrentX2


!============================================================
! Standard Acquisition Service
! Global Definitions
!
!
!============================================================

!============================================================
! Acquisition command
!
! ACQ_RUN:
!
! 0 = Stop
! 1 = Run
!
! This is a command, NOT actual running state.
!============================================================

GLOBAL INT ACQ_RUN = 0


!============================================================
! Acquisition state
!
! 0 = IDLE
! 1 = STARTING
! 2 = RUNNING
! 3 = STOPPING
! 4 = ERROR
!============================================================

GLOBAL INT ACQ_STATE = 0


!============================================================
! Latest published block
!
! ACQ_PUBLISHED_SEQ is the final COMMIT marker.
! Host should watch this variable.
!============================================================

GLOBAL INT ACQ_PUBLISHED_SESSION = 0
GLOBAL INT ACQ_PUBLISHED_BLOCK = 0
GLOBAL INT ACQ_PUBLISHED_COUNT = 0
GLOBAL INT ACQ_PUBLISHED_PARTIAL = 0
GLOBAL INT ACQ_PUBLISHED_SEQ = 0


!============================================================
! Per-block metadata
!
! Index:
!
!     0 = ACQ_DATA_1
!     1 = ACQ_DATA_2
!     ...
!     4 = ACQ_DATA_5
!
! VALID_COUNT:
!     Number of valid samples PER CHANNEL.
!
! PARTIAL:
!     0 = full block
!     1 = partial block
!
! SEQ:
!     Block publication sequence.
!     0 = block currently invalid / not published
!
! SESSION:
!     Session that owns this block.
!============================================================

GLOBAL INT ACQ_BLOCK_VALID_COUNT(5)

GLOBAL INT ACQ_BLOCK_PARTIAL(5)

GLOBAL INT ACQ_BLOCK_SEQ(5)

GLOBAL INT ACQ_BLOCK_SESSION(5)


!============================================================
! Physical acquisition buffers
!
! Current profile:
!
! Channel 0 = FACC(X)
! Channel 1 = FVEL(X)
! Channel 2 = MOTOR_CURRENT
! Channel 3 = MOTOR_TEMPERATURE
! Channel 4 = CURRFORCE
! Channel 5 = FPOS(X)
!
!
! IMPORTANT:
!
! Keep literal dimensions here.
!
! If ACQ_CHANNEL_COUNT changes, these declarations and the
! DC channel list below must be changed together.
!
! 6 channels:
!
!     100000 / 6 = 16666
!
!============================================================

GLOBAL REAL ACQ_DATA_1(6)(16666)

GLOBAL REAL ACQ_DATA_2(6)(16666)

GLOBAL REAL ACQ_DATA_3(6)(16666)

GLOBAL REAL ACQ_DATA_4(6)(16666)

GLOBAL REAL ACQ_DATA_5(6)(16666)

