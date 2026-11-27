const DEFAULTS=[
{id:1,name:"Red Lamp",type:"Light",icon:"bulb",color:"#ff5b61",state:false},
{id:2,name:"Green Lamp",type:"Light",icon:"bulb",color:"#27d17f",state:false},
{id:3,name:"Blue Lamp",type:"Light",icon:"bulb",color:"#438cff",state:false},
{id:4,name:"Yellow Lamp",type:"Light",icon:"bulb",color:"#f1c232",state:false}];
const KEY="smart-home-config-v1";
const ICONS={
bulb:'<path d="M9 18h6M10 22h4M8.2 14.5a8 8 0 1 1 7.6 0c-.9.5-1.4 1.4-1.5 2.5h-4.6c-.1-1.1-.6-2-1.5-2.5Z"/>',
fan:'<circle cx="12" cy="12" r="2"/><path d="M12 10c-1-5 2-7 4-5 2 2 0 4-2 6M14 12c5-1 7 2 5 4-2 2-4 0-6-2M12 14c1 5-2 7-4 5-2-2 0-4 2-6M10 12c-5 1-7-2-5-4 2-2 4 0 6 2"/>',
snow:'<rect x="5" y="3" width="14" height="18" rx="2"/><path d="M5 11h14M9 7h.01M9 15h.01"/>',
drop:'<path d="M12 2S5 10 5 15a7 7 0 0 0 14 0c0-5-7-13-7-13Z"/>',
plug:'<path d="M8 3v6M16 3v6M6 9h12v2a6 6 0 0 1-6 6v4"/>',
home:'<path d="m3 11 9-8 9 8M5 10v11h14V10M9 21v-7h6v7"/>'};
let channels=loadConfig(),deviceUrl="",wsClient=null;
function loadConfig(){try{const a=JSON.parse(localStorage.getItem(KEY));if(Array.isArray(a)&&a.length===4)return DEFAULTS.map((d,i)=>({...d,...a[i],id:d.id,state:false}));}catch(e){console.warn(e);}return DEFAULTS.map(x=>({...x}));}
function saveLocal(){localStorage.setItem(KEY,JSON.stringify(channels.map(({state,...x})=>x)));}
function esc(v){return String(v).replace(/[&<>"']/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#39;"}[c]));}
function icon(name){return '<svg viewBox="0 0 24 24" aria-hidden="true">'+(ICONS[name]||ICONS.bulb)+'</svg>';}
function render(){
document.getElementById("appliance-grid").innerHTML=channels.map(c=>`<article class="appliance-card" style="--accent:${c.color}">
<div class="appliance-top"><span class="appliance-icon">${icon(c.icon)}</span><span class="state-pill ${c.state?"is-on":""}">${c.state?"ON":"OFF"}</span></div>
<h3>${esc(c.name)}</h3><p class="channel-label">Channel ${c.id} · ${esc(c.type)}</p>
<button class="toggle-button ${c.state?"is-on":""}" data-toggle="${c.id}" aria-pressed="${c.state}">Turn ${c.state?"OFF":"ON"}</button></article>`).join("");
document.getElementById("config-list").innerHTML=channels.map(c=>`<button class="config-item" type="button" data-edit="${c.id}">
<span class="config-copy"><strong>Channel ${c.id} — ${esc(c.name)}</strong><small>Type: ${esc(c.type)} · Color: ${c.color.toUpperCase()}</small></span><span class="config-edit">✎</span></button>`).join("");
}
function status(message,online=false){document.getElementById("connection-note").textContent=message;const p=document.getElementById("connection-status");p.innerHTML="<i></i>"+(online?"Connected":"Preview / Offline");p.style.color=online?"#25d994":"#f0c46d";p.style.background=online?"#103a35":"#4a3921";}
async function setState(id,state){const c=channels.find(x=>x.id===id);if(!c)return;if(deviceUrl){try{const r=await fetch(deviceUrl+"/api/relay/"+id,{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({state})});if(!r.ok)throw Error("HTTP "+r.status);c.state=state;render();}catch(e){status("Command failed: "+e.message);return;}}else{c.state=state;render();}}
function edit(id){const c=channels.find(x=>x.id===id);if(!c)return;document.getElementById("edit-channel").value=id;document.getElementById("edit-name").value=c.name;document.getElementById("edit-type").value=c.type;document.getElementById("edit-icon").value=c.icon;document.getElementById("edit-color").value=c.color;document.getElementById("dialog-title").textContent="Edit channel "+id;document.getElementById("edit-dialog").showModal();}
async function connect(url){deviceUrl=url.replace(/\/$/,"");document.getElementById("device-ip").textContent=deviceUrl;status("Connecting to ESP32…");try{const r=await fetch(deviceUrl+"/api/status");if(!r.ok)throw Error("Status endpoint unavailable");const data=await r.json();status("ESP32 connected",true);if(Array.isArray(data.channels))data.channels.forEach((v,i)=>{if(channels[i])channels[i].state=Boolean(v);});const cfg=await fetch(deviceUrl+"/api/config");if(cfg.ok){const d=await cfg.json();if(Array.isArray(d.channels))d.channels.forEach((v,i)=>{if(channels[i])channels[i]={...channels[i],...v,state:channels[i].state};});}render();
if(wsClient){try{wsClient.close();}catch(e){}}const ws=new WebSocket(deviceUrl.replace(/^http/,"ws")+"/ws");wsClient=ws;
ws.onmessage=e=>{try{const d=JSON.parse(e.data);if(Array.isArray(d.channels)){d.channels.forEach((v,i)=>{if(channels[i])channels[i].state=Boolean(v);});render();}else if(Number.isInteger(d.channel)&&channels[d.channel-1]){channels[d.channel-1].state=Boolean(d.state);render();}}catch(err){console.warn(err);}};
ws.onclose=()=>status("WebSocket disconnected");}catch(e){status("Could not connect: "+e.message);}}
document.getElementById("appliance-grid").addEventListener("click",e=>{const b=e.target.closest("[data-toggle]");if(!b)return;const c=channels.find(x=>x.id===Number(b.dataset.toggle));if(c)setState(c.id,!c.state);});
document.getElementById("config-list").addEventListener("click",e=>{const b=e.target.closest("[data-edit]");if(b)edit(Number(b.dataset.edit));});
document.getElementById("open-config").addEventListener("click",()=>edit(1));
document.getElementById("all-off").addEventListener("click",async()=>{if(deviceUrl){try{const r=await fetch(deviceUrl+"/api/all-off",{method:"POST"});if(!r.ok)throw Error("HTTP "+r.status);channels.forEach(c=>c.state=false);render();}catch(e){status("All OFF failed: "+e.message);}}else channels.forEach(c=>setState(c.id,false));});
document.getElementById("edit-form").addEventListener("submit",async e=>{e.preventDefault();const c=channels.find(x=>x.id===Number(document.getElementById("edit-channel").value));if(!c)return;c.name=document.getElementById("edit-name").value.trim();c.type=document.getElementById("edit-type").value;c.icon=document.getElementById("edit-icon").value;c.color=document.getElementById("edit-color").value;saveLocal();if(deviceUrl){try{const r=await fetch(deviceUrl+"/api/config",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify({channels:channels.map(({state,...x})=>x)})});if(!r.ok)status("Saved locally; ESP32 config save failed.");else status("Configuration saved to ESP32",true);}catch(err){status("Saved locally; ESP32 unavailable.");}}render();document.getElementById("edit-dialog").close();});
document.getElementById("cancel-edit").addEventListener("click",()=>document.getElementById("edit-dialog").close());
document.getElementById("connection-settings").addEventListener("click",()=>document.getElementById("connection-dialog").showModal());
document.getElementById("cancel-connection").addEventListener("click",()=>document.getElementById("connection-dialog").close());
document.getElementById("connection-form").addEventListener("submit",e=>{e.preventDefault();const u=document.getElementById("device-url").value.trim();if(!/^https?:\/\//i.test(u)){document.getElementById("connect-message").textContent="Enter a valid http:// or https:// address.";return;}document.getElementById("connection-dialog").close();connect(u);});
render();
// Auto-connect when dashboard is opened directly from the ESP32 web server.
if (location.hostname && location.hostname !== "localhost" && location.hostname !== "127.0.0.1") {
  connect(location.origin);
  document.getElementById("device-url").value = location.origin;
}
