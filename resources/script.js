var canvasElement = document.querySelector("canvas");

var Module = {
  canvas: canvasElement,
};

window.addEventListener("beforeunload", function() {
    FS.syncfs(false, function(err) {
        if (err) console.error("Final save error", err);
    });
});