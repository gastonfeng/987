unit hz1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ComCtrls, Buttons;

type
  TForm1 = class(TForm)
    Label1: TLabel;
    openhzk: TOpenDialog;
    filein: TOpenDialog;
    fileout: TOpenDialog;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    ComboBox1: TComboBox;
    ComboBox2: TComboBox;
    ComboBox3: TComboBox;
    ComboBox4: TComboBox;
    ComboBox5: TComboBox;
    go: TButton;
    help: TButton;
    Memo1: TMemo;
    StatusBar1: TStatusBar;
    SpeedButton1: TSpeedButton;
    SpeedButton2: TSpeedButton;
    SpeedButton3: TSpeedButton;
    Label6: TLabel;
    ComboBox6: TComboBox;
    procedure SpeedButton1Click(Sender: TObject);
    procedure SpeedButton3Click(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure openhzkCanClose(Sender: TObject; var CanClose: Boolean);
    procedure fileinCanClose(Sender: TObject; var CanClose: Boolean);
    procedure fileoutCanClose(Sender: TObject; var CanClose: Boolean);
    procedure goClick(Sender: TObject);
    procedure FormActivate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.SpeedButton1Click(Sender: TObject);
begin
openhzk.Execute;
end;

procedure TForm1.SpeedButton3Click(Sender: TObject);
begin
filein.Execute;
end;

procedure TForm1.SpeedButton2Click(Sender: TObject);
begin
fileout.Execute;
end;

procedure TForm1.openhzkCanClose(Sender: TObject; var CanClose: Boolean);
begin
ComboBox2.Items:=openhzk.Files;
ComboBox2.Text:=ComboBox2.Items[0];
end;

procedure TForm1.fileinCanClose(Sender: TObject; var CanClose: Boolean);
begin
ComboBox3.Items:=filein.Files;
ComboBox3.Text:=ComboBox3.Items[0];
end;

procedure TForm1.fileoutCanClose(Sender: TObject; var CanClose: Boolean);
begin
ComboBox4.Items:=fileout.Files;
ComboBox4.Text:=ComboBox4.Items[0];
end;

procedure TForm1.goClick(Sender: TObject);
var hzk:integer;
    fi,fo:Textfile;
    num,i,j,k:integer;
    flag:boolean;
    a,c:char;
    zz:array[0..1000] of char;
    buf,buf1:array[0..31] of byte;
    index:longint;
    x:byte;
begin
  num:=0;
  if not FileExists(ComboBox2.text) then
    begin
      Messagedlg('请输入正确的汉字库文件名及路径!',mterror,[mbOK],0);
      exit;
    end;
  if not FileExists(ComboBox3.text) then
    begin
      Messagedlg('请输入正确的源程序文件名及路径!',mterror,[mbOK],0);
      exit;
    end;
  Assignfile(fi,combobox3.text);
  reset(fi);
  while not Eof(fi) do
    begin
      Read(fi,a);
      if a <>'"' then continue;
      repeat
      Read(fi,a);
      if a>=chr(160) then
        begin
          Read(fi,c);
          flag:=false;
          for i:=0 to num div 2 do
          begin
            if(zz[i*2]=a) then
              if zz[i*2+1]=c then
              begin
              flag:=true;break;
              end;
          end;
          if flag=false then
          begin
          if c>=chr(160) then
            begin
              zz[num]:=a;
              inc(num);
              zz[num]:=c;
              inc(num);
            end;
          end;
        end;
      until a='"';
  end;
  assignfile(fo,combobox4.text);
  rewrite(fo);
  writeln(fo,'/*本文件由字模提取程序生成,');
  writeln(fo,'如果有任何问题,请联系作者fengjt@371.net.');
  writeln(fo,'程序可从http://fjt.nethome.com.cn免费下载.*/');
  writeln(fo,'/*汉字索引对照表*/');
  write(fo,'code unsigned int CHINESEINDEX[]={');
  for i:=0 to (num-1) div 2 do
  begin
    write(fo,ord(zz[i*2])*256+ord(zz[i*2+1]));
    write(fo,',');
  end;
  writeln(fo,'};');
  writeln(fo,'/*汉字字摸点阵表*/');
  write(fo,'code unsigned char CHINESEDOT[');
  write(fo,num div 2);
  write(fo,'][32]={');
  hzk:=fileopen(combobox2.text,fmopenread);
  for i:=0 to (num-1) div 2 do
  begin
    a:=zz[i*2];
    c:=zz[i*2+1];
    index:=((ord(a)-161)and 127)*94+((ord(c)-161)and 127);
    fileseek(hzk,index*32,0);
    fileRead(hzk,buf[0],32);
    if combobox6.itemindex=0 then
    begin
      for j:=0 to 7 do
      begin
        for k:=0 to 7 do
        begin
          x:=128 shr j;
          buf1[j]:=buf1[j] div 2;
          if x and buf[k*2]<>0 then buf1[j]:=buf1[j]+128;
        end;
      end;
      for j:=8 to 15 do
      begin
        for k:=0 to 7 do
        begin
          x:=128 shr (j-8);
          buf1[j]:=buf1[j] div 2;
          if x and buf[k*2+1]<>0 then buf1[j]:=buf1[j]+128;
        end;
      end;
      for j:=16 to 23 do
      begin
        for k:=8 to 15 do
        begin
          x:=128 shr (j-16);
          buf1[j]:=buf1[j] div 2;
          if x and buf[k*2]<>0 then buf1[j]:=buf1[j]+128;
        end;
      end;
      for j:=24 to 31 do
      begin
        for k:=8 to 15 do
        begin
          x:=128 shr(j-24);
          buf1[j]:=buf1[j] div 2;
          if x and buf[k*2+1]<>0 then buf1[j]:=buf1[j]+128;
        end;
      end;
      for j:=0 to 31 do buf[j]:=buf1[j];
    end;
    write(fo,'{');
    for j:= 0 to 30 do
    begin
      write(fo,buf[j]);
      write(fo,',');
    end;
    write(fo,buf[31]);
    if i<(num-1) div 2 then writeln(fo,'},')
    else write(fo,'}');
  end;
  write(fo,'};');
  closefile(fi);
  closefile(fo);
  fileclose(hzk);
  memo1.Text:='完成.';
end;

procedure TForm1.FormActivate(Sender: TObject);
begin
Combobox6.ItemIndex:=0;
Combobox6.Text:=combobox6.Items[combobox6.itemindex];
end;

end.
